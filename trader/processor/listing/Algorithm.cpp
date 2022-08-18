//
// Created by Вадим Проскурин on 16.01.2022.
//

#include "Algorithm.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"
#include "base/Settings.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Report.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/base/Position.hpp"
#include "exchanger/base/OrderCreator.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

using namespace listing;

Algorithm* Algorithm::create(const Settings& settings) {
    auto algorithm = new Algorithm(settings);
    return algorithm;
}

Algorithm::Algorithm(const Settings& settings)
    : _settings(settings)
{
}

Algorithm::~Algorithm() {
    if (_position != nullptr) {
        const Symbol symbol = _position->symbol();

        Exchanger().unlistenTickers(symbol);

        delete _position;
        _position = nullptr;
    }
}

bool Algorithm::init(const Symbol& symbol) {
    _position = Position::create(_settings.username(), symbol);
    Exchanger().listenTickers(symbol);
    return true;
}

bool Algorithm::execute() {
    const Symbol symbol = _position->symbol();

    const PriceWrapper* price = Exchanger().price(symbol);
    if (price == nullptr)
        return false;

    const Ticker& ticker = price->ticker();
    if (ticker.time > Time().ms() - Timer::sMinute) {
        Logger::info(util::format("ticker(%s) ask(%s) bid(%s)", ticker.symbol.c_str(), ticker.bestAskPrice.c_str(), ticker.bestBidPrice.c_str()));
        return true;
    }

    return false;
    // создадим позицию и выйдем
    // if (tryOpen())
    //    return false;

    // закроем позицию, если успешно тогда алгоритм удалится
    // return tryClose();
}

bool Algorithm::tryOpen() {
    if (_position->has())
        return false;

    const Symbol symbol = _position->symbol();

    // цена для открытии позиции должна быть известна
    const Price price = symbol.price(Buy);
    if (price == Decimal::Zero)
        return false;

    // если уже имеем эту монету, просто создаем позицию
    const Quantity& balance = symbol.baseAsset().balance();
    if (balance > Decimal::Zero) {
        _position->setSide(Buy);
        _position->setBaseQuantity(balance);
        _position->setQuoteQuantity(balance * price);
    } else {
        // создадим реквест
        OrderRequest request;
        request.symbol = symbol;
        request.side = Buy;
        request.quantity = Exchanger().roundQuantity(0, symbol);
        request.policy = OrderRequest::None;

        // создадим заказ
        Position open;
        if (not OrderCreator::create(request, open, _settings.isRelease()))
            return false;

        _position->copy(open);
    }

    // сохраним позицию
    _position->setTime(Time().ms());
    _position->save(_settings.isRelease());

    Logger::info(util::format("opened on price %s", price.c_str()));
    return true;
}

bool Algorithm::tryClose() {
    if (not _position->has())
        return false;

    const Symbol symbol = _position->symbol();

    // получим врапер с ценой
    PriceWrapper* wrapper = Exchanger().price(symbol);
    if (wrapper == nullptr)
        return false;

    // достанем последний тикер
    const Ticker& ticker = wrapper->ticker();
    if (ticker.time < Time().ms() - Timer::sSecond * 5)
        return false;

    // тикер должен существовать
    Price price = ticker.bestBidPrice;
    if (price == Decimal::Zero)
        return false;

    // проверим, что мы в профите
    Quantity profit = _position->profit(price);
    if (profit < Decimal::Zero)
        return false;

    // TODO: проверить что цена выше минимальной

    // созданим реквест
    OrderRequest request;
    request.symbol = _position->symbol();
    request.side = _position->revert();
    request.quantity = _position->baseQuantity();

    // создадим заказ
    Position close;
    if (not OrderCreator::create(request, close, _settings.isRelease()))
        return false;

    // создадим отчет
    Report report(*_position, close);

    // удалим позицию
    _position->remove(_settings.isRelease());

    Logger::info(util::format("closed with profit %s", report.profit.c_str()));
    return true;
}
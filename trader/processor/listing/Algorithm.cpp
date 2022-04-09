//
// Created by Вадим Проскурин on 16.01.2022.
//

#include "Algorithm.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"
#include "base/Position.hpp"
#include "base/Settings.hpp"
#include "base/Report.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Symbol.hpp"
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
    delete _position;
    _position = nullptr;
}

bool Algorithm::init(const Symbol& symbol) {
    _position = Position::create(_settings.username(), symbol.id());
    Exchanger().listenTickers(symbol);
    return true;
}

bool Algorithm::execute() {
    const Symbol symbol = _position->symbol();

    const PriceWrapper* price = Exchanger().price(symbol);
    const Ticker& ticker = price->ticker();
    if (ticker.time > Time().ms() - Timer::sMinute) {
        Logger::info(util::format("ticker(%s) ask(%f) bid(%f)", ticker.symbol.c_str(), ticker.bestAskPrice, ticker.bestBidPrice));
        Exchanger().unlistenTickers(symbol);
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

    // получение актуальной цены
    if (not Exchanger().loadPrice(symbol))
        return false;

    const Price price = symbol.price(Buy);
    const Quantity& balance = symbol.baseAsset().balance();

    // проверим, что средняя цена нам уже известна
    if (price == 0.0)
        return false;

    // если уже имеем эту монету, просто создаем позицию
    if (balance > 0) {
        _position->setSide(Buy);
        _position->setBaseQuantity(balance);
        _position->setQuoteQuantity(balance * price);
    } else {
        // создадим реквест
        OrderRequest request;
        request.symbol = symbol;
        request.side = Buy;
        request.quantity = Exchanger().roundQuantity(0, symbol);

        // создадим заказ
        Position open;
        if (not createOrder(request, open))
            return false;

        _position->copy(open);
    }

    // сохраним позицию
    _position->setTime(Time().ms());
    _position->save(_settings.isRelease());

    // TODO: поддержать отписку
    // подписываемся на тикеры
    Exchanger().listenTickers(symbol);

    Logger::info(util::format("opened on price %f", price));
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
    if (price == 0.0)
        return false;

    // проверим, что мы в профите
    Quantity profit = _position->profit(price);
    if (profit < 0)
        return false;

    // TODO: проверить что цена выше минимальной

    // созданим реквест
    OrderRequest request;
    request.symbol = _position->symbol();
    request.side = _position->revert();
    request.quantity = _position->baseQuantity();

    // создадим заказ
    Position close;
    if (not createOrder(request, close))
        return false;

    // создадим отчет
    Report report(*_position, close);

    // удалим позицию
    _position->remove(_settings.isRelease());

    Logger::info(util::format("closed with profit %f", report.profit));
    return true;
}

bool Algorithm::createOrder(OrderRequest& request, Position& result) const {
    if (request.side == OrderSide::Invalid)
        return false;

    if (not _settings.isRelease()) {
        const Asset& asset = OrderUtil::usedAsset(request.side, request.symbol);
        if (asset.balance() < request.required())
            return false;

        result.setSide(request.side);
        result.setBaseQuantity(Exchanger().roundQuantity(request.quantity, request.symbol));
        result.setQuoteQuantity(result.baseQuantity() * request.symbol.price(request.side));
        result.operate();
        return true;
    }

    // создание заказа
    const OrderWrapper* order = Exchanger().createOrder(request);
    if (order == nullptr)
        return false;

    result.copy(*order);
    return true;
}
//
// Created by Вадим Проскурин on 16.01.2022.
//

#include "Algorithm.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"
#include "base/Position.hpp"
#include "base/Settings.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

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
    _position = Position::create(_settings.username, symbol.id());
    return true;
}

bool Algorithm::execute() {
    if (tryOpen())
        return false;

    return tryClose();
}

bool Algorithm::tryOpen() {
    if (_position->has())
        return false;

    const Symbol symbol = _position->symbol();

    // update price
    if (not Exchanger().loadPrice(symbol))
        return false;

    // get price and balance
    const Price price = symbol.price(Buy);
    const Quantity& balance = symbol.baseAsset().balance();

    if (price == 0.0)
        return false;

    // already have coins
    if (balance > 0) {
        _position->setSide(Buy);
        _position->setBaseQuantity(balance);
        _position->setQuoteQuantity(balance * price);
        Logger::info(util::format("opened on price %f", price));
    }

    _position->save(_settings.isRelease());

    // todo: unhandle
    // listen tickers
    Exchanger().listenTickers(symbol);

    return true;
}

bool Algorithm::tryClose() {
    if (not _position->has())
        return false;

    const Symbol symbol = _position->symbol();

    // get price wrapper
    PriceWrapper* wrapper = Exchanger().price(symbol);
    if (wrapper == nullptr)
        return false;

    // get actual ticker
    const Ticker& ticker = wrapper->ticker();
    if (ticker.time < Time().ms() - Timer::sSecond * 5)
        return false;

    // price must exist
    Price price = ticker.bestBidPrice;
    if (price == 0.0)
        return false;

    // close with profit
    Quantity profit = _position->profit(price);
    if (profit < 0)
        return false;

    Logger::info(util::format("closed with profit %f", profit));
    _position->remove(_settings.isRelease());
    return true;
}


/*bool Strategy::tryOpen() {
    if (_new_symbols.empty())
        return false;

    // !for test: with single position
    if (_position != nullptr)
        return false;

    // !for test: pick first symbol
    const Symbol& symbol = _new_symbols.front();

    // refresh prices
    if (not Exchanger().loadPrice(symbol))
        return false;

    // get price
    const PriceWrapper* price = Exchanger().price(symbol);
    if (price == nullptr)
        return false;

    _max_price = price->get(OrderSide::Buy);
    if (_max_price <= std::numeric_limits<double>::epsilon())
        return false;

    const std::string user = username();

    _position = Position::create(user, symbol);
    _position->setSymbol(symbol);
    _position->setSide(OrderSide::Buy);
    _position->setBaseQuantity(Exchanger().roundQuantity(0, symbol));
    _position->setQuoteQuantity(_position->baseQuantity() * _max_price);
    _position->operate();

    // listen ticker, todo: disconnect
    Exchanger().listenTickers(symbol);

    // write log
    std::string text = util::format("Buy new listed %s for price %f", symbol.c_str(), _position->price());
    Logger::info(text);

    // send log
    // protocol::Event::add(user, text);
    return true;
}

bool Strategy::tryClose() {
    if (_position == nullptr)
        return false;

    // get price
    const PriceWrapper* price = Exchanger().price(_position->symbol());
    if (price == nullptr)
        return false;

    Price current_price = price->get(OrderSide::Sell);
    _max_price = std::max(_max_price, current_price);

    Price k = (_max_price - current_price) / (_max_price - _position->price());
    if (k < 0.3)
        return false;

    // write log
    std::string text = util::format("Sell new listed %s for price %f (profit %f)", _position->symbol().c_str(), _position->price(), _position->profit(current_price));
    Logger::info(text);

    delete _position;
    _position = nullptr;
    return true;
}*/
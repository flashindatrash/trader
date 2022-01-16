//
// Created by Вадим Проскурин on 16.01.2022.
//

#include "Algorithm.hpp"
#include "core/Config.hpp"
#include "base/Position.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Symbol.hpp"

using namespace listing;

Algorithm* Algorithm::create(const core::Config& config) {
    auto algorithm = new Algorithm(config);
    return algorithm;
}

Algorithm::Algorithm(const core::Config& config)
    : _config(config)
{
}

Algorithm::~Algorithm() {
    delete _position;
    _position = nullptr;
}

bool Algorithm::init(const Symbol& symbol) {
    // maybe not needed
    if (not Exchanger().loadPrice(symbol))
        return false;

    _position = Position::create(_config.asString("REDIS_USERNAME"), symbol.id());
    return true;
}

bool Algorithm::execute() {
    return false;
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
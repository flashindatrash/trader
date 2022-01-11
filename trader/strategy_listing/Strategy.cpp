//
// Created by Вадим Проскурин on 11.01.2022.
//

#include "Strategy.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"
#include "protocol/Event.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/Exchanger.hpp"
#include "base/Position.hpp"

using namespace listing;

bool Strategy::init(const core::Config& config) {
    _config = config;

    Symbol::onAdded.connect(std::bind(&Strategy::tryOpen, this, std::placeholders::_1));
    Time().onTick.connect(std::bind(&Strategy::tick, this, std::placeholders::_1));
    return true;
}

bool Strategy::isRunning() const {
    return true;
}

void Strategy::tick(time_t ms) {
    tryClose();

    // refresh trading pairs
    Exchanger().loadPairs();
}

void Strategy::tryOpen(const Symbol& symbol) {
    // interesting pairs with usd
    if (not symbol.quoteAsset().isUSD())
        return;

    // for test with single position
    if (_position != nullptr)
        return;

    std::string username = _config.asString("REDIS_USERNAME");

    _position = Position::create(Position::key(username, symbol.id()));
    _position->setSymbol(symbol);
    _position->setSide(OrderSide::Buy);
    /*_position->setBaseQuantity(Exchanger().roundQuantity(request.quantity, request.symbol));
    _position->setQuoteQuantity(result.baseQuantity() * Context::current->price(request.side));*/
    _position->operate();

    // write log
    std::string text = util::format("New listing %s", symbol.c_str());
    Logger::info(text);

    // send log
    protocol::Event::add(username, text);
}

void Strategy::tryClose() {

}
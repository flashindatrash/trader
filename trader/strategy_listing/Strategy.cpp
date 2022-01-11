//
// Created by Вадим Проскурин on 11.01.2022.
//

#include "Strategy.hpp"
#include "Logger.hpp"
#include "Time.hpp"
#include "Event.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/Exchanger.hpp"

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

    std::string text = util::format("New listing %s", symbol.c_str());
    Logger::info(text);

    std::string username = _config.asString("REDIS_USERNAME");
    protocol::Event::add(username, text);
}

void Strategy::tryClose() {

}
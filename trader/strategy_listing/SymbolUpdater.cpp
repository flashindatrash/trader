//
// Created by Вадим Проскурин on 15.01.2022.
//

#include "SymbolUpdater.hpp"
#include "core/Time.hpp"
#include "core/Logger.hpp"
#include "exchanger/Exchanger.hpp"

using namespace listing;

SymbolUpdater::~SymbolUpdater() {
    disconnect();
}

void SymbolUpdater::connect() {
    _connector = Symbol::onAdded.connect(std::bind(&SymbolUpdater::found, this, std::placeholders::_1));
}

void SymbolUpdater::disconnect() {
    _connector = Symbol::onAdded.disconnect(_connector);
}

const SymbolUpdater::Data& SymbolUpdater::vector() const {
    return _symbols;
}

bool SymbolUpdater::request() {
    _symbols.emplace_back(Symbol("QIUSDT"));
    return true;

    static time_t _last = 0;

    time_t time_passed = Time().ms() - _last;

    // antispam
    if (time_passed < 2000)
        return true;

    // connect before request
    connect();

    // request new pairs
    bool loaded = Exchanger().loadPairs();
    _last = Time().ms();

    // disconnect handler
    disconnect();

    return loaded && time_passed < Timer::sMinute;
}

void SymbolUpdater::found(const Symbol& symbol) {
    Logger::info(util::format("New listed symbol: %s", symbol.c_str()));

    // interesting pairs with usdt
    if (symbol.quoteAsset().id() == Asset::USDT.id())
        _symbols.push_back(symbol);
}
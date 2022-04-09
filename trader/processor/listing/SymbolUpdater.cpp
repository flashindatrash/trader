//
// Created by Вадим Проскурин on 15.01.2022.
//

#include "SymbolUpdater.hpp"
#include "SymbolFrequency.hpp"
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
    // how often
    if (not SymbolFrequency::request())
        return false;

    // connect & request & disconnect
    connect();
    bool status = Exchanger().loadPairs();
    disconnect();

    return status;
}

void SymbolUpdater::found(const Symbol& symbol) {
    Logger::info(util::format("New listed symbol: %s", symbol.c_str()));
    _symbols.push_back(symbol);
}
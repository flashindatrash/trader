//
// Created by Вадим Проскурин on 15.01.2022.
//

#include "ListedSymbols.hpp"
#include "core/Time.hpp"
#include "exchanger/Exchanger.hpp"

using namespace listing;

ListedSymbols ListedSymbols::find() {
    ListedSymbols result;
    result.proceed();
    return result;
}

ListedSymbols::~ListedSymbols() {
    disconnect();
}

void ListedSymbols::connect() {
    _connector = Symbol::onAdded.connect(std::bind(&ListedSymbols::found, this, std::placeholders::_1));
}

void ListedSymbols::disconnect() {
    _connector = Symbol::onAdded.disconnect(_connector);
}

ListedSymbols::Status ListedSymbols::status() const {
    return _status;
}

const ListedSymbols::Data& ListedSymbols::vector() const {
    return _symbols;
}

void ListedSymbols::proceed() {
    static time_t _last = 0;

    // antispam
    if (_last + 2000 > Time().ms())
        return;

    // connect before request
    connect();

    // request new pairs
    if (Exchanger().loadPairs())
        _status = _symbols.empty() ? Empty : Ok;
    else
        _status = Failed;

    // disconnect handler
    disconnect();

    _last = Time().ms();
}

void ListedSymbols::found(const Symbol& symbol) {
    // interesting pairs with usdt
    // if (symbol.quoteAsset().id() == Asset::USDT.id())
    _symbols.push_back(symbol);
}
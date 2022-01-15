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

    time_t time_passed = Time().ms() - _last;

    // antispam
    if (time_passed < 2000)
        return;

    // connect before request
    connect();

    // request new pairs
    if (Exchanger().loadPairs()) {
        if (_symbols.empty()) {
            _status = Empty;
        } else if (time_passed > Timer::sMinute) {
            _status = Failed;
        } else {
            _status = Ok;
        }
    } else
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
#include "Symbol.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

Symbol* Symbol::create() {
    auto* wrapper = new Symbol();
    return wrapper;
}

Symbol::Symbol(const std::string& symbol) {
    setId(symbol);

    const Symbol* pair = Exchanger().pair(symbol);
    if (pair == nullptr)
        return;

    _base = pair->baseAsset();
    _quote = pair->quoteAsset();
}

Symbol::Symbol(const Asset& base, const Asset& quote) {
    set(base, quote);
}

void Symbol::set(const Asset& base, const Asset& quote) {
    setId(base.id() + quote.id());

    _base = base;
    _quote = quote;
}

const Asset& Symbol::baseAsset() const {
    return _base;
}

const Asset& Symbol::quoteAsset() const {
    return _quote;
}

const Price& Symbol::price() const {
    return Exchanger().price(*this)->get();
}

Price Symbol::price(double quantity) const {
    return price() * quantity;
}

const char* Symbol::c_str() const {
    return _identifier.c_str();
}

Symbol::operator std::string() const {
    return _identifier;
}

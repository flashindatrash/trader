#include "Symbol.hpp"
#include "proxy/Exchanger.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "util/StringUtil.hpp"

Asset::Asset(const std::string& asset) {
    setId(util::uppercase(asset.c_str()));
}

const char* Asset::c_str() const {
    return _identifier.c_str();
}

Asset::operator std::string() const {
    return _identifier;
}

const double& Asset::getBalance() const {
    return Exchanger().balance(*this)->get();
}

Symbol* Symbol::create() {
    Symbol* wrapper = new Symbol();
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

Symbol::Symbol(Asset base, Asset quote) {
    set(base, quote);
}

void Symbol::set(Asset base, Asset quote) {
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

const Price& Symbol::getPrice() const {
    return Exchanger().price(*this)->get();
}

const Price Symbol::getPrice(double quantity) const {
    return getPrice() * quantity;
}

const char* Symbol::c_str() const {
    return _identifier.c_str();
}

Symbol::operator std::string() const {
    return _identifier;
}

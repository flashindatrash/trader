#include "proxy/BinanceAccount.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "proxy/BinanceExchangeInfo.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"
#include "util/StringUtil.hpp"

Asset::Asset()
    : std::string("")
{
}

Asset::Asset(const std::string& asset)
    : std::string(util::uppercase(asset.c_str()))
{
}

const double Asset::getBalance() const {
    return SAccount().getBalance(*this);
}

Symbol::Symbol()
    : std::string("")
{
}

Symbol::Symbol(const std::string& symbol)
    : std::string(symbol)
{
    const BinanceSymbolData& info = getInfo();
    _base = info.baseAsset;
    _quote = info.quoteAsset;
}

Symbol::Symbol(Asset base, Asset second)
    : _base(base)
    , _quote(second)
    , std::string(base + second)
{
}

const Asset& Symbol::baseAsset() const {
    return _base;
}

const Asset& Symbol::quoteAsset() const {
    return _quote;
}

const BinanceSymbolData& Symbol::getInfo() const {
    return SExchangeInfo().getSymbolInfo(*this);
}

const Price Symbol::getPrice() const {
    if (const PriceContainer* wrapper = Exchanger().price(*this))
        return wrapper->getCurrent();
    return 0.0;
}

const Price Symbol::getPrice(double quantity) const {
    return getPrice() * quantity;
}

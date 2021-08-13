#include "proxy/BinanceAccount.hpp"
#include "proxy/BinancePrices.hpp"
#include "proxy/BinanceExchangeInfo.hpp"
#include "exchanger/wrapper/TradeSymbol.hpp"
#include "exchanger/wrapper/PriceSymbol.hpp"
#include "util/StringUtil.hpp"

TradeAsset::TradeAsset()
    : std::string("")
{
}

TradeAsset::TradeAsset(const std::string& asset)
    : std::string(util::uppercase(asset.c_str()))
{
}

const double TradeAsset::getBalance() const {
    return SAccount().getBalance(*this);
}

TradeSymbol::TradeSymbol()
    : std::string("")
{
}

TradeSymbol::TradeSymbol(const std::string& symbol)
    : std::string(symbol)
{
    const BinanceSymbolData& info = getInfo();
    _base = info.baseAsset;
    _quote = info.quoteAsset;
}

TradeSymbol::TradeSymbol(TradeAsset base, TradeAsset second)
    : _base(base)
    , _quote(second)
    , std::string(base + second)
{
}

const TradeAsset& TradeSymbol::baseAsset() const {
    return _base;
}

const TradeAsset& TradeSymbol::quoteAsset() const {
    return _quote;
}

const BinanceSymbolData& TradeSymbol::getInfo() const {
    return SExchangeInfo().getSymbolInfo(*this);
}

const Price TradeSymbol::getPrice() const {
    if (const PriceSymbol* wrapper = SPrices().getPrice(*this))
        return wrapper->getCurrent();
    return 0.0;
}

const Price TradeSymbol::getPrice(double quantity) const {
    return getPrice() * quantity;
}

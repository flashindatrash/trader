#include "BinanceSymbol.hpp"

#include "binacpp_utils.h"

#include "proxy/BinanceAccount.hpp"
#include "proxy/BinancePrices.hpp"
#include "proxy/BinanceExchangeInfo.hpp"

BinanceAsset::BinanceAsset()
    : std::string("")
{
}

BinanceAsset::BinanceAsset(const std::string& asset)
    : std::string(asset)
{
    string_toupper(*this);
}

const double BinanceAsset::getBalance() const {
    return SBinanceAccount().getBalance(*this);
}

BinanceSymbol::BinanceSymbol()
    : std::string("")
{
}

BinanceSymbol::BinanceSymbol(const std::string& symbol)
    : std::string(symbol)
{
    const BinanceSymbolData& info = getInfo();
    _base = info.baseAsset;
    _quote = info.quoteAsset;
}

BinanceSymbol::BinanceSymbol(BinanceAsset base, BinanceAsset second)
    : _base(base)
    , _quote(second)
    , std::string(base + second)
{
}

const BinanceAsset& BinanceSymbol::baseAsset() const {
    return _base;
}

const BinanceAsset& BinanceSymbol::quoteAsset() const {
    return _quote;
}

const double BinanceSymbol::getPrice() const {
    return SBinancePrices().getPrice(*this);
}

const BinanceSymbolData& BinanceSymbol::getInfo() const {
    return SBinanceExchangeInfo().getSymbolInfo(*this);
}

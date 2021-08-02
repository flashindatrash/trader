#include "BinanceSymbol.hpp"

#include "binacpp_utils.h"

#include "proxy/BinanceAccount.hpp"
#include "proxy/BinancePrices.hpp"

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

BinanceSymbol::BinanceSymbol(BinanceAsset base, BinanceAsset second)
    : _base(base)
    , _second(second)
    , std::string(base + second)
{
}

const BinanceAsset& BinanceSymbol::baseAsset() const {
    return _base;
}

const BinanceAsset& BinanceSymbol::secondAsset() const {
    return _second;
}

const double BinanceSymbol::getPrice() const {
    return SBinancePrices().getPrice(*this);
}

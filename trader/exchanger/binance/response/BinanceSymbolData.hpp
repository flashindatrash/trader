#pragma once

#include <vector>
#include "exchanger/base/Decimal.hpp"

namespace Json {
    class Value;
}

struct BinanceSymbolData {
public: // struct
    struct BaseFilter {
        bool has = false;
    };

    struct MinNotional : public BaseFilter {
        bool applyToMarket = false;
        int avgPriceMins = 0;
        Decimal minNotional;
    };

    struct LotSize : public BaseFilter {
        Decimal maxQty;
        Decimal minQty;
        Decimal stepSize;
    };

public: // methods
    BinanceSymbolData() = default;
    BinanceSymbolData(const Json::Value& json);

    bool hasOrderType(const std::string& type) const;

public: // vars
    std::string symbol;
    std::string status;

    std::string baseAsset;
    std::string quoteAsset;

    int baseAssetPrecision = 0;
    int baseCommissionPrecision = 0;

    int quoteAssetPrecision = 0;
    int quoteCommissionPrecision = 0;
    bool quoteOrderQtyMarketAllowed = false;
    int quotePrecision = 0;

    bool icebergAllowed = false;
    bool isMarginTradingAllowed = false;
    bool isSpotTradingAllowed = false;
    bool ocoAllowed = false;

    MinNotional minNotional;
    LotSize lotSize;

private: // vars
    std::vector<std::string> _orderTypes;
    std::vector<std::string> _permissions;
};


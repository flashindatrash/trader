#pragma once

#include <vector>
#include <string>
#include "exchanger/wrapper/ExchangeWrapper.hpp"

struct BinanceSymbolData : public SymbolData {
public: // struct
    struct BaseFilter {
        bool has = false;
    };

    struct MinNotional : public BaseFilter {
        bool applyToMarket = false;
        int avgPriceMins = 0;
        double minNotional = 0.0;
    };

    struct LotSize : public BaseFilter {
        double maxQty = 0.0;
        double minQty = 0.0;
        double stepSize = 0.0;
    };

public: // methods
    BinanceSymbolData() = default;
    BinanceSymbolData(const Json::Value& json);

    bool hasOrderType(const OrderType& type) const override;

public: // vars
    std::string symbol = "";
    std::string status = "";

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


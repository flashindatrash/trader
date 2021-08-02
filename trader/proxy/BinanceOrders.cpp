#include "BinanceOrders.hpp"

#include "binacpp.h"
#include "Logger.hpp"
#include "BinanceConfig.hpp"
#include "BinanceSymbol.hpp"
#include "data/BinanceSymbolData.hpp"

std::vector<BinanceOrderData> BinanceOrders::getAllOrders(const BinanceSymbol& symbol, int limit/* = 0*/) const {
    Json::Value result;
    BinaCPP::get_allOrders(symbol.c_str(), 0, limit, BINANCE_RECV_WINDOW, result);

    std::vector<BinanceOrderData> vec;
    if (!result.isArray())
        return vec;

    for (uint i = 0; i < result.size(); ++i)
        vec.push_back(result[i]);

    return vec;
}

const BinanceOrderData BinanceOrders::createOrder(const BinanceSymbol& symbol, const std::string& side, double quantity) const {
    const BinanceSymbolData& info = symbol.getInfo();
    if (not info.hasOrderType("MARKET")) {
        log("can't trade symbol %s on MARKET", symbol.c_str());
        return BinanceOrderData();
    }

    // todo: недостающая проверка для маркета
    // info.minNotional.applyToMarket &&
    // средняя цена за последнии {info.minNotional.avgPriceMins} минут (если 0 - то последняя) * quantity >= info.minNotional.minNotional
    // (*пока нет метода узнать среднюю цену)

    Json::Value result;
    BinaCPP::send_order(symbol.c_str(), side.c_str(), "MARKET", "GTC", quantity , 0, "", 0, 0, BINANCE_TEST_MODE, BINANCE_RECV_WINDOW, result);
    return BinanceOrderData(result);
}

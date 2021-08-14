#include "binacpp.h"
#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinancePrices.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/binance/response/BinanceErrorData.hpp"
#include "exchanger/binance/response/BinancePriceStatisticsData.hpp"

const BinancePriceStatisticsData& BinancePrices::getStats(const Symbol& symbol) {
    static const BinancePriceStatisticsData sEmpty;

    PriceWrapper* wrapper = Exchanger().price(symbol);
    if (wrapper == nullptr)
        return sEmpty;

    BinancePriceStatisticsData& stats = wrapper->getStats();

    // lazy update from api
    time_t now = STime().getCurrent();
    if (now - BinanceTime::sMinute * 30 > stats.closeTime) {
        Json::Value result;
        BinaCPP::get_24hr(symbol.c_str(), result);

        BinanceErrorData error(result);
        if (error.has()) {
            Logger::error(error.msg.c_str());
            return stats;
        }

        stats = BinancePriceStatisticsData(result);
    }

    return stats;
}

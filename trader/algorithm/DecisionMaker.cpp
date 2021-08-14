#include "Logger.hpp"
#include "DecisionMaker.hpp"
#include "proxy/BinancePrices.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/SideEnum.hpp"
#include "exchanger/binance/response/BinancePriceStatisticsData.hpp"

DecisionMaker::DecisionMaker(const Symbol& symbol)
    : _symbol(symbol)
{
}

double DecisionMaker::factor(const SideEnum& side, int based_on) const {
    if (side == SideEnum::Invalid)
        return 0.0;

    double result = 1.0;

    if (has(based_on, DayChange)) {
        // увеличиваем/понижаем рейтинг при отрицательном дневном росте/падении
        const BinancePriceStatisticsData& stats = SPrices().getStats(_symbol);
        if (stats.priceChangePercent != 0.0)
            result *= 1.0 + stats.priceChangePercent / 100.0 * (side == SideEnum::Buy ? 1 : -1);
    }

    if (has(based_on, Balance)) {
        // понижаем рейтинг на сделки, при балансе ассета < 50%
        static double sBalanceThreshold = 0.5;
        double baseQty = _symbol.getPrice(_symbol.baseAsset().getBalance());
        double quoteQty = _symbol.quoteAsset().getBalance();
        result *= std::abs((side == SideEnum::Sell ? baseQty : quoteQty) / ((baseQty + quoteQty) * sBalanceThreshold));
    }

    return result;
}

bool DecisionMaker::has(int mask, BasedOn value) const {
    return 0 != (mask & value);
}

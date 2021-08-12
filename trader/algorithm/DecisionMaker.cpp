#include "Logger.hpp"
#include "proxy/BinancePrices.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "data/BinanceSideEnum.hpp"
#include "data/BinancePriceStatisticsData.hpp"
#include "algorithm/DecisionMaker.hpp"

// мин % соотношение, может требовать х2 взависимости от факторов
static float sRate = 0.005f;

double DecisionMaker::sDefaultRef = 0.0;

DecisionMaker::DecisionMaker(const TradeSymbol& symbol)
    : _symbol(symbol)
{
}

double DecisionMaker::calc(const BinanceSideEnum& side, int based_on) const {
    double result = 1.0;

    if (has(based_on, DayChange)) {
        // увеличиваем/понижаем рейтинг при отрицательном дневном росте/падении
        const BinancePriceStatisticsData& stats = SPrices().getStats(_symbol);
        if (stats.priceChangePercent != 0.0)
            result *= 1.0 + stats.priceChangePercent / 100.0 * (side == BinanceSideEnum::Buy ? 1 : -1);
    }

    if (has(based_on, Balance)) {
        // понижаем рейтинг на сделки, при балансе ассета < 50%
        static double sBalanceThreshold = 0.5;
        double baseQty = _symbol.getPrice(_symbol.baseAsset().getBalance());
        double quoteQty = _symbol.quoteAsset().getBalance();
        result *= std::abs((side == BinanceSideEnum::Sell ? baseQty : quoteQty) / ((baseQty + quoteQty) * sBalanceThreshold));
    }

    return result;
}

bool DecisionMaker::make(Change change, int based_on, double& factor) const {
    factor = 0.0;

    double rate = change / sRate;
    if (std::abs(rate) < 1.0)
        return false;

    factor = calc(rate, based_on);
    if (factor < 0.0)
        logic_error("factor of DecisionMaker must be positive");

    return std::abs(rate) * factor >= 1.0;
}

bool DecisionMaker::has(int mask, BasedOn value) const {
    return 0 != (mask & value);
}

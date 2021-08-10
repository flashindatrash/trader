#include "Logger.hpp"
#include "proxy/BinancePrices.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "data/BinanceSideEnum.hpp"
#include "data/BinancePriceStatisticsData.hpp"
#include "algorithm/DecisionMaker.hpp"

// мин % соотношение, может требовать х2 взависимости от факторов
static float sMinRate = 0.0035f;

double DecisionMaker::sDefaultRef = 0.0;

DecisionMaker::DecisionMaker(const TradeSymbol& symbol)
    : _symbol(symbol)
{
}

double DecisionMaker::factor(double base, int based_on) const {
    BinanceSideEnum side(base);

    if (has(based_on, DayChange)) {
        // увеличиваем/понижаем рейтинг при отрицательном дневном росте/падении
        const BinancePriceStatisticsData& stats = SPrices().getStats(_symbol);
        if (stats.priceChangePercent != 0.0)
            base *= 1.0 + stats.priceChangePercent / 100.0 * (side == BinanceSideEnum::Buy ? 1 : -1);
    }

    if (has(based_on, Balance)) {
        // понижаем рейтинг на сделки, при балансе ассета < 25%
        static double sBalanceThreshold = 0.25;
        double baseQty = _symbol.getPrice(_symbol.baseAsset().getBalance());
        double quoteQty = _symbol.quoteAsset().getBalance();
        base *= std::abs((side == BinanceSideEnum::Sell ? baseQty : quoteQty) / ((baseQty + quoteQty) * sBalanceThreshold));
    }

    return base;
}

bool DecisionMaker::make(Change base, int based_on, double& out) const {
    out = 0.0;

    double rate = base / sMinRate;
    if (std::abs(rate) < 1.0)
        return false;

    out = factor(rate, based_on);
    if (not Changes::equal(rate, out))
        logic_error("DecisionMaker::make gets wrong factor");

    return std::abs(out) >= 1.0;
}

bool DecisionMaker::has(int mask, BasedOn value) const {
    return 0 != (mask & value);
}

#include "proxy/BinancePrices.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "data/BinanceSideEnum.hpp"
#include "data/BinancePriceStatisticsData.hpp"
#include "algorithm/DecisionMaker.hpp"

// мин % соотношение, может требовать х2 взависимости от факторов
static float sMinRate = 0.0035f;

DecisionMaker::DecisionMaker(const TradeSymbol& symbol)
    : _symbol(symbol)
{
}

double DecisionMaker::factor(double base, int based_on) const {
    BinanceSideEnum side(base);

    if (has(based_on, DayChange)) {
        const BinancePriceStatisticsData& stats = SPrices().getStats(_symbol);
        if (stats.priceChangePercent != 0.0)
            base *= 1.0 + stats.priceChangePercent / 100.0 * (side == BinanceSideEnum::Buy ? 1 : -1);
    }

    if (has(based_on, Balance)) {
        double baseQty = _symbol.getPrice(_symbol.baseAsset().getBalance());
        double quoteQty = _symbol.quoteAsset().getBalance();
        base *= (side == BinanceSideEnum::Sell ? baseQty : quoteQty) / ((baseQty + quoteQty) * 0.5);
    }

    return base;
}

bool DecisionMaker::make(double base, int based_on) const {
    return std::abs(base) > 1.0 && std::abs(factor(base / sMinRate, based_on)) > 1.0;
}

bool DecisionMaker::has(int mask, BasedOn value) const {
    return 0 != (mask & value);
}

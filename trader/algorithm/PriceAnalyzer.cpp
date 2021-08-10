#include "wrapper/KlineHistory.hpp"
#include "data/BinanceKlineData.hpp"
#include "algorithm/PriceAnalyzer.hpp"

PriceAnalyzer::PriceAnalyzer(const KlineHistory& history)
    : _history(history)
{
}

Change PriceAnalyzer::getStablePriceChange(time_t since) const {
    if (_history.klines().empty())
        return 0.0;

    if (_history.klines().size() == 1)
        return PriceRange(_history.back().priceOpen, _history.back().priceClose).change();

    Change total = 0.0;
    for (auto it = _history.klines().crbegin(); it < _history.klines().crend() - 1; ++it) {
        const BinanceKlineData& right = *it;
        const BinanceKlineData& left = *(it + 1);

        // todo: у нас нет данных о изменении цен внутри одной свечи
        // заменить на timeClose
        if (right.timeStart < since)
            break;

        // todo: hlco4 and etc
        const Change change = PriceRange(left.priceClose, right.priceClose).change();
        if ((total > 0.0 && change < 0.0) || (total < 0.0 && change > 0.0))
            break;

        total += change;
    }

    return total;
}


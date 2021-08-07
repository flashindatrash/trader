#include "wrapper/KlineHistory.hpp"
#include "data/BinanceKlineData.hpp"
#include "algorithm/PriceAnalyzer.hpp"
#include "util/PriceUtil.hpp"

PriceAnalyzer::PriceAnalyzer(const KlineHistory& history)
    : _history(history)
{
}

double PriceAnalyzer::getChangeSince(time_t time) const {
    double total = 0.0;
    for (auto it = _history.klines().crbegin(); it < _history.klines().crend() - 1; ++it) {
        const BinanceKlineData& current = *it;
        const BinanceKlineData& prev = *(it + 1);

        if (current.timeStart < time)
            break;

        // todo: hlco4 and etc
        const double change = util::get_percent(prev.ohlc4(), current.ohlc4());

        if ((total > 0.0 && change < 0.0) || (total < 0.0 && change > 0.0))
            break;

        total += change;
    }
    return total;
}


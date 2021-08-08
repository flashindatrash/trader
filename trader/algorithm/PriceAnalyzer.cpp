#include "wrapper/KlineHistory.hpp"
#include "data/BinanceKlineData.hpp"
#include "algorithm/PriceAnalyzer.hpp"
#include "util/PriceUtil.hpp"

PriceAnalyzer::PriceAnalyzer(const KlineHistory& history)
    : _history(history)
{
}

double PriceAnalyzer::getChangeSince(time_t time) const {
    if (_history.klines().empty())
        return 0.0;

    if (_history.klines().size() == 1)
        return util::get_percent(_history.back().priceOpen, _history.back().priceClose);

    double total = 0.0;
    for (auto it = _history.klines().crbegin(); it < _history.klines().crend() - 1; ++it) {
        const BinanceKlineData& right = *it;
        const BinanceKlineData& left = *(it + 1);

        if (right.timeClose < time)
            break;

        // todo: hlco4 and etc
        const double change = util::get_percent(left.priceClose, right.priceClose);
        if ((total > 0.0 && change < 0.0) || (total < 0.0 && change > 0.0))
            break;

        total += change;
    }
    return total;
}


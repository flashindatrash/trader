#include "PriceContainer.hpp"
#include "proxy/BinanceTime.hpp"

PriceContainer* PriceContainer::create() {
    PriceContainer* wrapper = new PriceContainer();
    return wrapper;
}

void PriceContainer::add(Price price) {
    add(price, STime().getCurrent());
}

void PriceContainer::add(Price price, time_t time) {
    if (not _per_second.empty() && std::abs(time - _per_second.back().second) < BinanceTime::sSecond)
        return;

    // todo: чистить вектор (и поддержать per_minute, per_hour)
    _per_second.push_back(std::make_pair(price, time));
}

double PriceContainer::getCurrent() const {
    if (_per_second.empty())
        return 0.0;
    return _per_second.back().first;
}

double PriceContainer::getPriceBack(time_t interval) const {
    time_t time = STime().getCurrent() - interval;

    PriceTimePair d1;
    PriceTimePair d2 = _per_second.back();
    for (auto it = _per_second.rbegin(); it < _per_second.rend(); ++it) {
        d1 = *it;
        if (it->second <= time)
            break;
        d2 = *it;
    }

    time_t t1 = std::abs(d1.second - time);
    time_t t2 = std::abs(d2.second - time);
    return t1 < t2 ? d1.first : d2.first;
}

double PriceContainer::getPriceAverage(time_t interval) const {
    double price_back = getPriceBack(interval);
    double price_current = getPriceBack(0);

    // todo: не учитывает цены в интервале, так и должно ли
    return (price_back + price_current) / 2.0;
}

BinancePriceStatisticsData& PriceContainer::getStats() {
    return _per_day;
}

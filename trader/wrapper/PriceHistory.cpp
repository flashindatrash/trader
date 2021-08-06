#include "proxy/BinanceTime.hpp"
#include "wrapper/PriceHistory.hpp"

PriceHistory* PriceHistory::create() {
    PriceHistory* wrapper = new PriceHistory();
    return wrapper;
}

void PriceHistory::add(double price) {
    add(price, STime().getCurrent());
}

void PriceHistory::add(double price, time_t time) {
    if (not _per_second.empty() && std::abs(time - _per_second.back().second) < BinanceTime::sSecond)
        return;

    // todo: чистить вектор (и поддержать per_minute, per_hour)
    _per_second.push_back(std::make_pair(price, time));
}

double PriceHistory::getPriceBack(time_t interval) const {
    time_t time = STime().getCurrent() - interval;

    Data d1;
    Data d2 = _per_second.back();
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

double PriceHistory::getPriceAverage(time_t interval) const {
    double price_back = getPriceBack(interval);
    double price_current = getPriceBack(0);

    // todo: не учитывает цены в интервале, так и должно ли
    return (price_back + price_current) / 2.0;
}

float PriceHistory::getPriceChangePercent(time_t interval) const {
    double price_back = getPriceBack(interval);
    double price_current = getPriceBack(0);
    double price_diff = price_current - price_back;

    return price_diff / price_back;
}

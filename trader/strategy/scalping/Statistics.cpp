//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Statistics.hpp"

NS_USE

static const char* FIELD_PROFIT = "profit";

Statistics* Statistics::create(const db::Key& key, bool sync) {
    auto* statistics = new Statistics(key, sync);
    return statistics;
}

Statistics::Statistics(const db::Key& key, bool sync)
    : db::Object(key)
    , _sync(sync)
{
}

Quantity Statistics::addProfit(Quantity profit) {
    Quantity result = inc(FIELD_PROFIT, profit).asDouble();
    if (_sync) save();
    return result;
}
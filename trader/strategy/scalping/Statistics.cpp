//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Statistics.hpp"

NS_USE

static const char* FIELD_PROFIT = "profit";

Statistics* Statistics::create(const Symbol& pair, bool sync) {
    auto* statistics = new Statistics("stats:" + pair.id(), sync);
    return statistics;
}

Statistics::Statistics(const db::Key& key, bool sync)
    : db::Object(key)
    , _sync(sync)
{
}

Quantity Statistics::addProfit(Quantity profit) {
    Quantity result = inc("profit", profit).asDouble();
    if (_sync) save();
    return result;
}
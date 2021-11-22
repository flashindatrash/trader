//
// Created by Вадим Проскурин on 22.11.2021.
//

#include "Stats.hpp"

using namespace protocol;

static const char* FIELD_PROFIT = "profit";
static const char* FIELD_CHANGE = "change";

Stats::Stats(db::Key key)
    : db::Object(key)
{
}

void Stats::setProfit(double value) {
    inc(FIELD_PROFIT, value);
}

double Stats::profit() const {
    return get(FIELD_PROFIT).asDouble();
}

void Stats::setChange(double value) {
    inc(FIELD_CHANGE, value);
}

double Stats::change() const {
    return get(FIELD_CHANGE).asDouble();
}
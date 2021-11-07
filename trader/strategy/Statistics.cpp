//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Statistics.hpp"
#include "Report.hpp"
#include <utility>

NS_USE

static const char* FIELD_VERSION = "version";
static const char* FIELD_PROFIT = "profit";
static const char* FIELD_CHANGE = "change";
static const char* FIELD_BASE = "base";
static const char* FIELD_QUOTE = "quote";

Statistics* Statistics::create(const db::Key& key) {
    auto* statistics = new Statistics(key);
    return statistics;
}

Statistics::Statistics(const db::Key& key)
    : db::Object(key)
{
}

void Statistics::report(const Report& report) {
    inc(FIELD_PROFIT, report.profit);
    inc(FIELD_CHANGE, report.change);
    inc(FIELD_BASE, report.earn_base);
    inc(FIELD_QUOTE, report.earn_quote);
}

std::string Statistics::version() const {
    return get(FIELD_VERSION).asString();
}

void Statistics::setVersion(std::string value) {
    set(FIELD_VERSION, std::move(value));
}
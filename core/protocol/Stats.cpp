//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Stats.hpp"
#include "Storage.hpp"

static const char* FIELD_VERSION = "version";
static const char* FIELD_PROFIT = "profit";
static const char* FIELD_BASE = "base";
static const char* FIELD_QUOTE = "quote";

using namespace protocol;

std::string Stats::key(const std::string& username, const std::string& symbol) {
    return Storage::key(username, symbol, "stats");
}

Stats::Stats(const std::string& username, const std::string& symbol)
    : db::Object(key(username, symbol))
{
}

void Stats::setProfit(int64_t value) {
    inc(FIELD_PROFIT, (int)value);
}

int64_t Stats::profit() const {
    return get(FIELD_PROFIT).asInt();
}

void Stats::setEarnBase(int64_t value) {
    inc(FIELD_BASE, (int)value);
}

void Stats::setEarnQuote(int64_t value) {
    inc(FIELD_QUOTE, (int)value);
}

std::string Stats::version() const {
    return get(FIELD_VERSION).asString();
}

int64_t Stats::earnBase() const {
    return get(FIELD_BASE).asInt();
}

int64_t Stats::earnQuote() const {
    return get(FIELD_QUOTE).asInt();
}

void Stats::setVersion(std::string value) {
    set(FIELD_VERSION, std::move(value));
}
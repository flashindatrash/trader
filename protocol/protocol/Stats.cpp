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

void Stats::setProfit(double value) {
    inc(FIELD_PROFIT, value);
}

double Stats::profit() const {
    return get(FIELD_PROFIT).asDouble();
}

void Stats::setEarnBase(double value) {
    inc(FIELD_BASE, value);
}

void Stats::setEarnQuote(double value) {
    inc(FIELD_QUOTE, value);
}

std::string Stats::version() const {
    return get(FIELD_VERSION).asString();
}

double Stats::earnBase() const {
    return get(FIELD_BASE).asDouble();
}

double Stats::earnQuote() const {
    return get(FIELD_QUOTE).asDouble();
}

void Stats::setVersion(std::string value) {
    set(FIELD_VERSION, std::move(value));
}
//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Trader.hpp"
#include "Storage.hpp"

static const char* FIELD_VERSION = "version";
static const char* FIELD_PROFIT = "profit";
static const char* FIELD_CHANGE = "change";
static const char* FIELD_BASE = "base";
static const char* FIELD_QUOTE = "quote";

using namespace protocol;

Trader::Trader(const std::string& username, const std::string& symbol)
    : db::Object(Storage::key(username, symbol, "stats"))
{
}

void Trader::setProfit(double value) {
    inc(FIELD_PROFIT, value);
}

double Trader::profit() const {
    return get(FIELD_PROFIT).asDouble();
}

void Trader::setChange(double value) {
    inc(FIELD_CHANGE, value);
}

double Trader::change() const {
    return get(FIELD_CHANGE).asDouble();
}

void Trader::setEarnBase(double value) {
    inc(FIELD_BASE, value);
}

void Trader::setEarnQuote(double value) {
    inc(FIELD_QUOTE, value);
}

std::string Trader::version() const {
    return get(FIELD_VERSION).asString();
}

double Trader::earnBase() const {
    return get(FIELD_BASE).asDouble();
}

double Trader::earnQuote() const {
    return get(FIELD_QUOTE).asDouble();
}

void Trader::setVersion(std::string value) {
    set(FIELD_VERSION, std::move(value));
}
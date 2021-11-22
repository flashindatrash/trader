//
// Created by Вадим Проскурин on 28.08.2021.
//

#include "Pair.hpp"
#include "Storage.hpp"

static const char* FIELD_VERSION = "version";
static const char* FIELD_BASE = "base";
static const char* FIELD_QUOTE = "quote";

using namespace protocol;

Pair::Pair(const std::string& username, const std::string& symbol)
    : Stats(Storage::key(username, symbol, "stats"))
{
}

void Pair::setEarnBase(double value) {
    inc(FIELD_BASE, value);
}

void Pair::setEarnQuote(double value) {
    inc(FIELD_QUOTE, value);
}

std::string Pair::version() const {
    return get(FIELD_VERSION).asString();
}

double Pair::earnBase() const {
    return get(FIELD_BASE).asDouble();
}

double Pair::earnQuote() const {
    return get(FIELD_QUOTE).asDouble();
}

void Pair::setVersion(std::string value) {
    set(FIELD_VERSION, std::move(value));
}
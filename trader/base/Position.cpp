#include "Position.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

static const char* FIELD_SIDE = "side";
static const char* FIELD_SYMBOL = "symbol";
static const char* FIELD_BASE_QUANTITY = "base_quantity";
static const char* FIELD_QUOTE_QUANTITY = "quote_quantity";
static const char* FIELD_TIME = "time";
static const char* FIELD_COUNT = "count";

Position* Position::create(const std::string& username, const std::string& symbol) {
    auto* position = new Position(username, symbol);
    position->setSymbol(symbol);
    return position;
}

Position::Position(const std::string& username, const std::string& symbol)
    : protocol::Position(username, symbol)
{
}

OrderBase::Id Position::id() const {
    return _key;
}

Symbol Position::symbol() const {
    return get(FIELD_SYMBOL).asString();
}

void Position::setSymbol(const Symbol& value) {
    set(FIELD_SYMBOL, value.c_str());
}

OrderSide Position::side() const {
    OrderSide side = (OrderSide)get(FIELD_SIDE).asInt();
    if (side == Buy || side == Sell)
        return side;
    return Invalid;
}

void Position::setSide(OrderSide value) {
    set(FIELD_SIDE, (int)value);
}

Quantity Position::baseQuantity() const {
    return get(FIELD_BASE_QUANTITY).asDouble();
}

void Position::setBaseQuantity(Quantity value) {
    set(FIELD_BASE_QUANTITY, value);
}

Quantity Position::quoteQuantity() const {
    return get(FIELD_QUOTE_QUANTITY).asDouble();
}

void Position::setQuoteQuantity(Quantity value) {
    set(FIELD_QUOTE_QUANTITY, value);
}

time_t Position::time() const {
    std::string value = get(FIELD_TIME).asString();
    if (value.empty())
        return 0;
    return stol(value.substr(2));
}

void Position::setTime(time_t value) {
    set(FIELD_TIME, "t:" + std::to_string(value));
}

int Position::count() const {
    return std::max(1, get(FIELD_COUNT).asInt());
}

void Position::setCount(int value) {
    set(FIELD_COUNT, value);
}

OrderSide Position::revert() const {
    return OrderUtil::revert(side());
}

Quantity Position::profit(Price current) const {
    return distance(current) * baseQuantity() - fee() - fee(current * baseQuantity());
}

Price Position::distance(Price current) const {
    return OrderUtil::distance(side(), price(), current);
}

Change Position::change(Price current) const {
    return distance(current) / price();
}

void Position::copy(const OrderBase& ref) {
    setSymbol(ref.symbol());
    setSide(ref.side());
    setBaseQuantity(ref.baseQuantity());
    setQuoteQuantity(ref.quoteQuantity());
}

void Position::merge(const OrderBase& ref) {
    if (not has() || side() != ref.side())
        return;

    setBaseQuantity(baseQuantity() + ref.baseQuantity());
    setQuoteQuantity(quoteQuantity() + ref.quoteQuantity());
    setCount(count() + 1);
}

bool Position::remove(bool release) {
    if (not release) {
        _map.clear();
        return true;
    }
    return db::Object::remove();
}

bool Position::save(bool release) {
    return not release || db::Object::save();
}

bool Position::has() const {
    return side() != Invalid && baseQuantity() > 0.0 && quoteQuantity() > 0.0;
}
#include "Position.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

NS_USE

static const char* FIELD_SIDE = "side";
static const char* FIELD_SYMBOL = "symbol";
static const char* FIELD_BASE_QUANTITY = "base_quantity";
static const char* FIELD_QUOTE_QUANTITY = "quote_quantity";

Position* Position::create(const db::Key& key) {
    auto* position = new Position(key);
    return position;
}

Position::Position(const db::Key& key)
    : db::Object(key)
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
    return (OrderSide)get(FIELD_SIDE).asInt();
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

bool Position::closable() const {
    return OrderUtil::isEnough(symbol(), OrderUtil::revert(side()), baseQuantity());
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
}

bool Position::has() const {
    return side() != OrderSide::Invalid && baseQuantity() > 0.0 && quoteQuantity() > 0.0;
}
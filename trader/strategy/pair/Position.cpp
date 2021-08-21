#include "Position.hpp"
#include <global.hpp>
#include "exchanger/wrapper/OrderWrapper.hpp"

static const char* FIELD_SYMBOL = "symbol";
static const char* FIELD_SIDE = "side";
static const char* FIELD_PRICE = "price";
static const char* FIELD_QUANTITY = "quantity";

NS_USE

Position::Position(Id id)
    : database::Object("position:" + id)
{
    setId(id);
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

Price Position::price() const {
    return get(FIELD_PRICE).asDouble();
}

void Position::setPrice(const Price& value) {
    set(FIELD_PRICE, value);
}

Quantity Position::quantity() const {
    return get(FIELD_QUANTITY).asDouble();
}

void Position::setQuantity(const Quantity& value) {
    set(FIELD_QUANTITY, value);
}

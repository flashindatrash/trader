#include "Positions.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

NS_USE

static const char* FIELD_PRICE = "price";
static const char* FIELD_SIDE = "side";
static const char* FIELD_QUANTITY = "quantity";

Position::Position(const db::Key& key)
    : db::Object(key)
{
}

const Price Position::price() const {
    return get(FIELD_PRICE).asDouble();
}

void Position::setPrice(Price value) {
    set(FIELD_PRICE, value);
}

const OrderSide Position::side() const {
    return (OrderSide)get(FIELD_SIDE).asInt();
}

void Position::setSide(OrderSide value) {
    set(FIELD_SIDE, (int)value);
}

const Quantity Position::quantity() const {
    return get(FIELD_QUANTITY).asDouble();
}

void Position::setQuantity(Quantity value) {
    set(FIELD_QUANTITY, value);
}

Positions* Positions::create(const Symbol& pair, bool sync) {
    Positions* positions = new Positions("test:" + pair.id(), sync);
    return positions;
}

Positions::Positions(const db::Key& key, bool sync)
    : db::ArrayAbstract<Position>(key)
    , _sync(sync)
{
    load();
}

bool Positions::proceed_push(Position& value) const {
    return not _sync || value.save();
}

bool Positions::proceed_erase(Position& value) const {
    return not _sync || value.remove();
}

bool Positions::proceed_load() const {
    return _sync;
}

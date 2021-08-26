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

const Quantity Position::profit(Price current) const {
    return current - price();
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
    return not proceed_sync() || value.save();
}

bool Positions::proceed_erase(Position& value) const {
    return not proceed_sync() || value.remove();
}

bool Positions::proceed_sync() const {
    return _sync;
}

void Positions::proceed_sort() {
    // todo
}

Positions::Predicate Predicates::combine(Positions::Predicate a, Positions::Predicate b) {
    return [a, b](const Position& value) {
        return a(value) && b(value);
    };
}

Positions::Predicate Predicates::greater(Price price) {
    return [price](const Position& position) {
        return position.price() > price;
    };
}

Positions::Predicate Predicates::less(Price price) {
    return [price](const Position& position) {
        return position.price() < price;
    };
}

Positions::Predicate Predicates::side(OrderSide side) {
    return [side](const Position& position) {
        return position.side() == side;
    };
}

bool Predicates::sell(const Position& position) {
    return position.side() == OrderSide::Sell;
}

bool Predicates::buy(const Position& position) {
    return position.side() == OrderSide::Buy;
}

bool Compares::max(const Position& a, const Position& b) {
    return b.price() > a.price();
}

bool Compares::min(const Position& a, const Position& b) {
    return b.price() < a.price();
}

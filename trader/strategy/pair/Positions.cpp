#include "Positions.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/Exchanger.hpp"

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

const Change Position::distance(Price current) const {
    if (side() == OrderSide::Sell) return price() - current;
    if (side() == OrderSide::Buy) return current - price();
    return 0.0;
}

Positions* Positions::create(const Symbol& pair, bool sync) {
    Positions* positions = new Positions("test:" + pair.id(), sync);
    return positions;
}

Positions::Positions(const db::Key& key, bool sync)
    : BaseClass(key)
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

bool Positions::create(const OrderRequest& request) {
    if (not request.isEnough())
        return false;

    const OrderWrapper* order = Exchanger().createOrder(request);
    if (order == nullptr)
        return false;

    Position position(order->id());
    position.setPrice(order->price());
    position.setQuantity(order->quantity());
    position.setSide(order->side());
    return push(position);
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

Positions::Compare Compares::distance(Price price) {
    return [price](const Position& a, const Position& b) {
        return b.distance(price) > a.distance(price);
    };
}

bool Compares::max(const Position& a, const Position& b) {
    return b.price() > a.price();
}

bool Compares::min(const Position& a, const Position& b) {
    return b.price() < a.price();
}

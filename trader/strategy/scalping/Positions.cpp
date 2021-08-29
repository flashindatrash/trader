#include <Time.hpp>
#include "Positions.hpp"

NS_USE

static const char* FIELD_SIDE = "side";
static const char* FIELD_BASE_QUANTITY = "base_quantity";
static const char* FIELD_QUOTE_QUANTITY = "quote_quantity";
static const char* FIELD_TIME = "time";

Position::Position(const db::Key& key)
    : db::Object(key)
{
}

OrderBase::Id Position::id() const {
    return _key;
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

time_t Position::time() const {
    return stol(get(FIELD_TIME).asString().substr(2));
}

void Position::setTime(time_t value) {
    set(FIELD_TIME, "t:" + std::to_string(value));
}

Positions* Positions::create(const db::Key& key, bool sync) {
    auto* positions = new Positions(key, sync);
    return positions;
}

Positions::Positions(const db::Key& key, bool sync)
    : BaseClass(key)
    , _sync(sync)
{
    load();
}

bool Positions::proceed_push(Position& value) const {
    // skip invalid Positions
    if (value.price() <= 0.0 || value.baseQuantity() <= 0.0 || value.side() == OrderSide::Invalid)
        return false;

    value.setTime(Time().ms());
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

bool Positions::copy(const OrderBase* order) {
    if (order == nullptr)
        return false;
    Position position(order->id());
    position.setQuoteQuantity(order->quoteQuantity());
    position.setBaseQuantity(order->baseQuantity());
    position.setSide(order->side());
    return push(position);
}

Positions::const_iterator Positions::last(OrderSide side) const {
    switch (side) {
    case OrderSide::Buy: return compare_if(Predicates::buy, Compares::min);
    case OrderSide::Sell: return compare_if(Predicates::sell, Compares::max);
    case OrderSide::Invalid: return cend();
    }
    return cend();
}

// ---------- Predicates ----------

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

// ---------- Compares ----------

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

// ---------- Summarizes ----------

Quantity Summarizes::expanses(const Position& position) {
    return position.expanses();
}

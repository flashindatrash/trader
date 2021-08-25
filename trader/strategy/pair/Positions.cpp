#include "Positions.hpp"

NS_USE

static const char* FIELD_PRICE = "price";

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

Positions* Positions::create(const Symbol& pair, bool sync) {
    Positions* positions = new Positions("positions:" + pair.id(), sync);
    return positions;
}

Positions::Positions(const db::Key& key, bool sync)
    : db::ArrayAbstract<Position>(key)
    , _sync(sync)
{
    load();
    Position pos("test");
    pos.setPrice(1.3);
    push(pos);
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

const Position* Positions::findProfitableFor(Price price) const {

    return nullptr;
}

/*static bool abs_compare_price(const db::Object& a, const db::Object& b)
{
    return std::abs(a.get(FIELD_PRICE).asDouble()) < std::abs(b.get(FIELD_PRICE).asDouble());
}*/

void Positions::max_element() {
    //double p_max = std::max_element(_array.begin(), _array.end(), abs_compare_price)->get(FIELD_PRICE).asDouble();
    //double p_min = std::min_element(_array.begin(), _array.end(), abs_compare_price)->get(FIELD_PRICE).asDouble();
    int i = 0;
}

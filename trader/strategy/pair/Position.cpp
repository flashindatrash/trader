#include "Position.hpp"

static const char* KEY = "pair";
static const char* FIELD_PRICE = "price";

NS_USE

Position* Position::get(const Id id) {
    Position* wrapper = new Position(id);
    if (not wrapper->load())
        SAFE_DELETE(wrapper);
    return wrapper;
}

Position::Position(Id id)
    : database::BaseWrapper(KEY, id)
{
}

Price Position::price() const {
    return _obj.get(FIELD_PRICE).asDouble();
}

void Position::setPrice(const Price value) {
    _obj.set(FIELD_PRICE, value);
}

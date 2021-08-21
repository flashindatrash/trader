#pragma once

#include "Defines.hpp"
#include "database/Object.hpp"
#include "exchanger/base/Identifier.hpp"

enum OrderSide : unsigned int;

NS_BEGIN
class Position : public database::Object, public Identifier {
public: // static
    WRAPPER_CREATE(Position)
    WRAPPER_BYID(Position)

public: // methods
    Symbol symbol() const;
    void setSymbol(const Symbol& value);

    OrderSide side() const;
    void setSide(OrderSide value);

    Price price() const;
    void setPrice(const Price& value);

    Quantity quantity() const;
    void setQuantity(const Quantity& value);

protected: // methods
    Position(const Id id);
};
NS_END

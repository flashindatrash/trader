#pragma once

#include "Defines.hpp"
#include "database/Array.hpp"
#include "exchanger/base/BaseTypes.hpp"

enum OrderSide : unsigned int;

NS_BEGIN
class Position : public db::Object {
public: // methods
    Position(const db::Key& key);

    const Price price() const;
    void setPrice(Price value);

    const OrderSide side() const;
    void setSide(OrderSide value);

    const Quantity quantity() const;
    void setQuantity(Quantity value);
};

class Positions : public db::ArrayAbstract<Position> {
public: // static
    static Positions* create(const Symbol& pair, bool sync);

protected: // methods
    Positions(const db::Key& key, bool sync);

    virtual bool proceed_push(Position& value) const override;
    virtual bool proceed_erase(Position& value) const override;
    virtual bool proceed_load() const override;

protected: // vars
    const bool _sync;
};
NS_END


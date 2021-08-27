#pragma once

#include "Defines.hpp"
#include "database/Array.hpp"
#include "exchanger/base/BaseTypes.hpp"

struct OrderRequest;
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

    const Change distance(Price current) const;
};

class Positions : public db::ArrayAbstract<Position> {
    typedef db::ArrayAbstract<Position> BaseClass;

public: // static
    static Positions* create(const Symbol& pair, bool sync);

public: // methods
    // create new order
    bool create(const OrderRequest& request);

    // last position by side
    const const_iterator last(OrderSide side) const;

protected: // methods
    Positions(const db::Key& key, bool sync);

    virtual bool proceed_push(Position& value) const override;
    virtual bool proceed_erase(Position& value) const override;
    virtual bool proceed_sync() const override;
    virtual void proceed_sort() override;

protected: // vars
    const bool _sync;
};

class Predicates {
public: // static
    // with arguments
    static Positions::Predicate combine(Positions::Predicate a, Positions::Predicate b);
    static Positions::Predicate greater(Price price);
    static Positions::Predicate less(Price price);
    static Positions::Predicate side(OrderSide side);
    // without arguments
    static bool sell(const Position& position);
    static bool buy(const Position& position);
};

class Compares {
public: // static
    // with arguments
    static Positions::Compare distance(Price price);
    // without arguments
    static bool max(const Position& a, const Position& b);
    static bool min(const Position& a, const Position& b);
};

class Summarizes {
public: // static
    static Quantity quantity(const Position& position);
};

NS_END


#pragma once

#include "Defines.hpp"
#include "database/Array.hpp"
#include "exchanger/base/OrderBase.hpp"

NS_BEGIN
class Position : public db::Object, public OrderBase {
public: // methods
    Position() = default;
    Position(const db::Key& key);
    Position(const OrderBase& order);

    void setSide(OrderSide value);
    void setSymbol(Symbol value);
    void setBaseQuantity(Quantity value);
    void setQuoteQuantity(Quantity value);
    void setTime(time_t value);

    Id id() const override;
    Symbol symbol() const override;
    OrderSide side() const override;
    Quantity baseQuantity() const override;
    Quantity quoteQuantity() const override;

    OrderSide revert() const;
    Price current() const;
    Quantity profit() const;
    Quantity profit(Price price) const;
    Change distance() const;
    Change distance(Price price) const;
};

class Positions : public db::ArrayAbstract<Position> {
    typedef db::ArrayAbstract<Position> BaseClass;

public: // static
    static Positions* create(const db::Key& key, bool sync);

public: // methods
    // last position by side
    const_iterator last(OrderSide side) const;

protected: // methods
    Positions(const db::Key& key, bool sync);

    bool proceed_push(Position& value) const override;
    bool proceed_erase(Position& value) const override;
    bool proceed_sync() const override;
    void proceed_sort() override;

protected: // vars
    const bool _sync;
};

class Predicates {
public: // static
    // with arguments
    static Positions::Predicate combine(Positions::Predicate a, Positions::Predicate b);
    static Positions::Predicate profitGreater(Quantity quantity);
    static Positions::Predicate side(OrderSide side);

    // without arguments
    static bool sell(const Position& position);
    static bool buy(const Position& position);
    static bool closable(const Position& position);
};

class Compares {
public: // static
    // without arguments
    static bool priceMax(const Position& a, const Position& b);
    static bool priceMin(const Position& a, const Position& b);
    static bool profitable(const Position& a, const Position& b);
    static bool losable(const Position& a, const Position& b);
};

class Summarizes {
public: // static
    // without arguments
    static Quantity profit(const Position& position);
};

NS_END


#pragma once

#include "Defines.hpp"
#include "database/Array.hpp"
#include "exchanger/base/OrderBase.hpp"

NS_BEGIN
class Position : public db::Object, public OrderBase {
public: // methods
    Position(const db::Key& key);

    void setSide(OrderSide value);
    void setBaseQuantity(Quantity value);
    void setQuoteQuantity(Quantity value);
    void setTime(time_t value);

    Id id() const override;
    OrderSide side() const override;
    Quantity baseQuantity() const override;
    Quantity quoteQuantity() const override;
    time_t time() const;

    Quantity profit(Price price) const;
};

class Positions : public db::ArrayAbstract<Position> {
    typedef db::ArrayAbstract<Position> BaseClass;

public: // static
    static Positions* create(const db::Key& key, bool sync);

public: // methods
    // create new order
    bool copy(const OrderBase* order);

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
    static Positions::Predicate priceGreater(Price price);
    static Positions::Predicate priceLess(Price price);
    static Positions::Predicate profitGreater(Price price, Quantity quantity);
    static Positions::Predicate side(OrderSide side);
    static Positions::Predicate closable(const Symbol& symbol);
    // without arguments
    static bool sell(const Position& position);
    static bool buy(const Position& position);
};

class Compares {
public: // static
    // with arguments
    static Positions::Compare profitable(Price price);
    static Positions::Compare losable(Price price);
    // without arguments
    static bool max(const Position& a, const Position& b);
    static bool min(const Position& a, const Position& b);
};

class Summarizes {
public: // static
    // with arguments
    static std::function<Quantity (const Position&)> profit(Price price);
    // without arguments
    static Quantity volume(const Position& position);
};

NS_END


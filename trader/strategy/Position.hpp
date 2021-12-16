#pragma once

#include "Defines.hpp"
#include "database/Array.hpp"
#include "exchanger/base/OrderBase.hpp"

NS_BEGIN
class Position : public db::Object, public OrderBase {
public: // static
    static Position* create(const db::Key& key);

public: // methods
    Position() = default;

    void setSide(OrderSide value);
    void setSymbol(const Symbol& value);
    void setBaseQuantity(Quantity value);
    void setQuoteQuantity(Quantity value);
    void setTime(time_t value);
    void setCount(int value);

    Id id() const override;
    Symbol symbol() const override;
    OrderSide side() const override;
    Quantity baseQuantity() const override;
    Quantity quoteQuantity() const override;
    time_t time() const;
    int count() const;

    OrderSide revert() const;
    Quantity profit(Price price) const;
    Price distance(Price price) const;
    Change change(Price price) const;

    void copy(const OrderBase& ref);
    void merge(const OrderBase& ref);

    bool has() const;

protected: // methods
    Position(const db::Key& key);

};

NS_END


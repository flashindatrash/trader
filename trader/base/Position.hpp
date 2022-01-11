#pragma once

#include "exchanger/base/OrderBase.hpp"
#include "protocol/Position.hpp"

struct OrderRequest;

class Position : public protocol::Position, public OrderBase {
public: // static
    static Position* create(const std::string& username, const std::string& symbol);

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

    bool remove(bool release);
    bool save(bool release);
    bool has() const;

protected: // methods
    Position(const std::string& username, const std::string& symbol);

};


#pragma once

#include "exchanger/base/Symbol.hpp"
#include "exchanger/base/OrderBase.hpp"

enum OrderType : unsigned int {
    Market
};

struct OrderStructure {
    OrderBase::Id id;
    OrderSide side = Invalid;
    Quantity quote_quantity = 0.0;
    Quantity base_quantity = 0.0;
};

struct OrderRequest {
    Symbol symbol;
    OrderSide side = Invalid;
    Quantity quantity = 0.0;
    OrderType type = Market;

    bool isEnough() const;
    static bool isEnough(const Symbol& symbol, OrderSide side, Quantity quantity);
};

class OrderWrapper : public OrderBase {
public: // static
    static OrderWrapper* create();

    // revert side
    static OrderSide revert(OrderSide side);

public: // methods
    void set(OrderStructure data);

    Id id() const override;
    OrderSide side() const override;
    Quantity baseQuantity() const override;
    Quantity quoteQuantity() const override;

protected: // methods
    OrderWrapper() = default;

protected: // vars
    OrderStructure _data;
};


#pragma once

#include "exchanger/base/Symbol.hpp"
#include "exchanger/base/OrderBase.hpp"

enum OrderType : unsigned int {
    Market
};

struct OrderStructure {
    OrderBase::Id id;
    Symbol symbol;
    OrderSide side = Invalid;
    Quantity quote_quantity = 0.0;
    Quantity base_quantity = 0.0;
};

struct OrderRequest {
    enum Policy : unsigned int {
        None,
        CheckBalance,
        RedeemSavings
    };

    Symbol symbol;
    OrderSide side = Invalid;
    Quantity quantity = 0.0;
    OrderType type = Market;
    unsigned int policy = CheckBalance | RedeemSavings;

    bool mask(Policy policy) const;
    Quantity required() const;
};

class OrderWrapper : public OrderBase {
public: // static
    static OrderWrapper* create();

public: // methods
    void set(OrderStructure data);

    Id id() const override;
    Symbol symbol() const override;
    OrderSide side() const override;
    Quantity baseQuantity() const override;
    Quantity quoteQuantity() const override;

protected: // methods
    OrderWrapper() = default;

protected: // vars
    OrderStructure _data;
};


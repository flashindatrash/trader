#pragma once

#include "exchanger/base/Symbol.hpp"
#include "exchanger/base/OrderBase.hpp"
#include "exchanger/base/Wallet.hpp"

enum OrderType : unsigned int {
    Market
};

struct OrderDetail {
    OrderBase::Id id;
    Symbol symbol;
    OrderSide side = Invalid;
    Quantity quote_quantity = 0.0;
    Quantity base_quantity = 0.0;
};

struct OrderRequest : public WalletRequest {
    Symbol symbol;
    OrderSide side = Invalid;
    Quantity quantity = 0.0;
    OrderType type = Market;

    Quantity required() const;
};

class OrderWrapper : public OrderBase {
public: // static
    static OrderWrapper* create();

public: // methods
    void set(OrderDetail data);

    Id id() const override;
    Symbol symbol() const override;
    OrderSide side() const override;
    Quantity baseQuantity() const override;
    Quantity quoteQuantity() const override;

protected: // methods
    OrderWrapper() = default;

protected: // vars
    OrderDetail _data;
};


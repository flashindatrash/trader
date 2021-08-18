#pragma once

#include <string>
#include "exchanger/wrapper/Symbol.hpp"

enum OrderType : unsigned int {
    Market
};

enum OrderSide : unsigned int {
    Invalid,
    Buy,
    Sell
};

struct Order {
    typedef std::string Id;

    Id id = "";
    OrderSide side = Invalid;
    Quantity quoute_quantity = 0.0;
    Quantity quantity = 0.0;
};

struct OrderRequest {
    Symbol symbol;
    OrderSide side = Invalid;
    Quantity quantity = 0.0;
    OrderType type = Market;

    bool isEnough() const;
    bool canTrade() const;
};

class OrderWrapper {
public: // static
    static OrderWrapper* create();

public: // methods
    void set(Order data);

    const Order::Id& id() const;
    const OrderSide& side() const;
    const Quantity& quantity() const;
    const Price price() const;

protected: // methods
    OrderWrapper() = default;

protected: // vars
    Order _data;
};


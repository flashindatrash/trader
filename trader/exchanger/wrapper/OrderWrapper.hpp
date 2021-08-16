#pragma once

#include <string>
#include "exchanger/base/ExchangerTypes.hpp"

enum OrderType : unsigned int {
    Market
};

enum OrderSide : unsigned int {
    Invalid,
    Buy,
    Sell
};

struct Order {
    std::string id = "";
    OrderSide side = Invalid;
    double quoute_quantity = 0.0;
    double quantity = 0.0;
};

struct OrderRequest {
    OrderSide side;
    double quantity = 0.0;
    OrderType type = Market;

    bool isEnough() const;
};

class OrderWrapper {
public: // static
    static OrderWrapper* create();

public: // methods
    void set(Order data);

    const std::string& getId() const;
    const OrderSide& side() const;
    const double& quantity() const;
    const Price getPrice() const;

protected: // methods
    OrderWrapper() = default;

protected: // vars
    Order _data;
};


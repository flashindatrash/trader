#pragma once

#include <string>
#include "exchanger/base/ExchangerTypes.hpp"

struct Order {
    std::string id = "";
    SideEnum side;
    double quoute_quantity = 0.0;
    double quantity = 0.0;
};

struct OrderRequest {
    enum Type {
        Market
    };

    SideEnum side;
    double quantity = 0.0;
    Type type = Market;

    bool isEnough() const;
};

class OrderWrapper {
public: // static
    static OrderWrapper* create();

public: // methods
    void set(Order data);

    const std::string& getId() const;
    const SideEnum& side() const;
    const double& quantity() const;
    const Price getPrice() const;

protected: // methods
    OrderWrapper() = default;

protected: // vars
    Order _data;
};


//
// Created by Вадим Проскурин on 27.08.2021.
//

#pragma once

#include <string>

typedef double Price;
typedef double Change;
typedef double Quantity;

class Symbol;

enum OrderSide : unsigned int {
    Invalid,
    Buy,
    Sell
};

class OrderBase {
public: // signature
    typedef std::string Id;

    virtual Id id() const = 0;
    virtual Symbol symbol() const = 0;
    virtual OrderSide side() const = 0;
    virtual Quantity baseQuantity() const = 0;
    virtual Quantity quoteQuantity() const = 0;

public: // methods
    Price price() const;
    Quantity fee() const;
    Quantity fee(Quantity quote) const;
    void operate() const;
};

class OrderUtil {
public:
    static Price distance(OrderSide side, Price price, Price current);
    static Quantity usedQuantity(OrderSide side, Quantity first, Quantity second);
    static Price price(Quantity baseQuantity, Quantity quoteQuantity);
    static OrderSide revert(OrderSide side);
    static bool isEnough(const Symbol& symbol, OrderSide side, Quantity quantity);
};
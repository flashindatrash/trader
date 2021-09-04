//
// Created by Вадим Проскурин on 27.08.2021.
//

#include "OrderBase.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Symbol.hpp"

Price OrderBase::price() const {
    return OrderUtil::price(baseQuantity(), quoteQuantity());
}

Price OrderBase::fee() const {
    return quoteQuantity() * Exchanger().fee();
}

Quantity OrderBase::usingQuantity() const {
    return OrderUtil::usingQuantity(side(), baseQuantity(), quoteQuantity());
}

Change OrderBase::distance(Price current) const {
    return OrderUtil::distance(side(), price(), current);
}

Change OrderUtil::change(const Price& left, const Price& right) {
    return (right - left) / left;
}

Change OrderUtil::changeAbs(const Price& left, const Price& right) {
    return std::abs(change(left, right));
}

Change OrderUtil::distance(OrderSide side, Price price, Price current) {
    switch (side) {
        case Buy: return current - price;
        case Sell: return price - current;
        case Invalid: return 0.0;
    }
    return 0.0;
}

Quantity OrderUtil::usingQuantity(OrderSide side, Quantity baseQuantity, Quantity quoteQuantity) {
    switch (side) {
        case Buy: return quoteQuantity;
        case Sell: return baseQuantity;
        case Invalid: return 0.0;
    }
    return 0.0;
}

Price OrderUtil::price(Quantity baseQuantity, Quantity quoteQuantity) {
    return quoteQuantity / baseQuantity;
}

OrderSide OrderUtil::revert(OrderSide side) {
    switch (side) {
        case Buy: return Sell;
        case Sell: return Buy;
        case Invalid: return Invalid;
    }
    return Invalid;
}

bool OrderUtil::isEnough(const Symbol& symbol, OrderSide side, Quantity quantity) {
    // допускаем погрешность
    static const double error = 1.3;

    Quantity balance = usingQuantity(side, symbol.baseAsset().getBalance(), symbol.quoteAsset().getBalance());
    Quantity cost = usingQuantity(side, quantity, symbol.price(quantity));
    return balance > cost * error;
}
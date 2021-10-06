//
// Created by Вадим Проскурин on 27.08.2021.
//

#include "OrderBase.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

Price OrderBase::price() const {
    return OrderUtil::price(baseQuantity(), quoteQuantity());
}

Quantity OrderBase::fee() const {
    return fee(quoteQuantity());
}

Quantity OrderBase::fee(Quantity quote) const {
    return quote * Exchanger().fee();
}

Price OrderUtil::distance(OrderSide side, Price price, Price current) {
    switch (side) {
        case Buy: return current - price;
        case Sell: return price - current;
        case Invalid: return 0.0;
    }
    return 0.0;
}

Quantity OrderUtil::usedQuantity(OrderSide side, Quantity baseQuantity, Quantity quoteQuantity) {
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

    Price price = Exchanger().price(symbol)->get(side);
    Quantity balance = usedQuantity(side, symbol.baseAsset().balance(), symbol.quoteAsset().balance());
    Quantity cost = usedQuantity(side, quantity, price * quantity);
    return balance > cost * error;
}
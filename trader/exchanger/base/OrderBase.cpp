//
// Created by Вадим Проскурин on 27.08.2021.
//

#include "OrderBase.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"

Price OrderBase::price() const {
    return OrderUtil::price(baseQuantity(), quoteQuantity());
}

Quantity OrderBase::fee() const {
    return fee(quoteQuantity());
}

Quantity OrderBase::fee(Quantity quote) const {
    return quote * Exchanger().fee();
}

void OrderBase::operate() const {
    BalanceWrapper* baseBalance = Exchanger().balance(symbol().baseAsset());
    BalanceWrapper* quoteBalance = Exchanger().balance(symbol().quoteAsset());
    if (baseBalance == nullptr || quoteBalance == nullptr)
        return;

    switch (side()) {
        case OrderSide::Buy: {
            baseBalance->gain(baseQuantity());
            quoteBalance->spend(quoteQuantity());
            break;
        }
        case OrderSide::Sell: {
            baseBalance->spend(baseQuantity());
            quoteBalance->gain(quoteQuantity());
            break;
        }
        case OrderSide::Invalid:
            break;
    }
}

Price OrderUtil::distance(OrderSide side, Price first, Price second) {
    switch (side) {
        case Buy: return second - first;
        case Sell: return first - second;
        case Invalid: return 0.0;
    }
    return 0.0;
}

Quantity OrderUtil::usedQuantity(OrderSide side, Quantity first, Quantity second) {
    switch (side) {
        case Buy: return second;
        case Sell: return first;
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
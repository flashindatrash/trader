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

const Asset& OrderUtil::usedAsset(OrderSide side, const Symbol& symbol) {
    switch (side) {
        case Buy: return symbol.quoteAsset();
        case Sell: return symbol.baseAsset();
        case Invalid: return Asset::Empty;
    }
    return Asset::Empty;
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
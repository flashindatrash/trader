//
// Created by Вадим Проскурин on 27.08.2021.
//

#include "OrderBase.hpp"

Price OrderBase::price() const {
    return OrderUtil::price(baseQuantity(), quoteQuantity());
}

Quantity OrderBase::expanses() const {
    return OrderUtil::usingQuantity(side(), baseQuantity(), quoteQuantity());
}

Change OrderBase::distance(Price current) const {
    return OrderUtil::distance(side(), price(), current);
}

Change OrderUtil::change(const Price& left, const Price& right) {
    return (right - left) / left;
}

Change OrderUtil::distance(OrderSide side, Price price, Price current) {
    switch (side) {
        case Buy: return current - price;
        case Sell: return price - current;
        case Invalid: return 0.0;
    }
}

Quantity OrderUtil::usingQuantity(OrderSide side, Quantity baseQuantity, Quantity quoteQuantity) {
    switch (side) {
        case Buy: return quoteQuantity;
        case Sell: return baseQuantity;
        case Invalid: return 0.0;
    }
}

Price OrderUtil::price(Quantity baseQuantity, Quantity quoteQuantity) {
    return quoteQuantity / baseQuantity;
}
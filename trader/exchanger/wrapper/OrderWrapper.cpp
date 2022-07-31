#include "OrderWrapper.hpp"
#include <utility>

OrderWrapper* OrderWrapper::create() {
    auto* wrapper = new OrderWrapper();
    return wrapper;
}

void OrderWrapper::set(OrderDetail data) {
    _data = std::move(data);
}

OrderBase::Id OrderWrapper::id() const {
    return _data.id;
}

Symbol OrderWrapper::symbol() const {
    return _data.symbol;
}

OrderSide OrderWrapper::side() const {
    return _data.side;
}

Quantity OrderWrapper::baseQuantity() const {
    return _data.base_quantity;
}

Quantity OrderWrapper::quoteQuantity() const {
    return _data.quote_quantity;
}

Quantity OrderRequest::required() const {
    return OrderUtil::usedQuantity(side, quantity, symbol.price(side) * quantity * 1.01);
}

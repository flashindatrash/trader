#include "OrderWrapper.hpp"
#include <utility>

OrderWrapper* OrderWrapper::create() {
    auto* wrapper = new OrderWrapper();
    return wrapper;
}

void OrderWrapper::set(OrderStructure data) {
    _data = std::move(data);
}

OrderBase::Id OrderWrapper::id() const {
    return _data.id;
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

bool OrderRequest::isEnough() const {
    return OrderUtil::isEnough(symbol, side, quantity);
}
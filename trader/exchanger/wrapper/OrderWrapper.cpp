#include "OrderWrapper.hpp"
#include "exchanger/base/Symbol.hpp"
#include <utility>

OrderWrapper* OrderWrapper::create()
{
    auto* wrapper = new OrderWrapper();
    return wrapper;
}

OrderSide OrderWrapper::revert(OrderSide side) {
    switch (side) {
    case OrderSide::Buy: return OrderSide::Sell;
    case OrderSide::Sell: return OrderSide::Buy;
    case OrderSide::Invalid: return OrderSide::Invalid;
    }
    return OrderSide::Invalid;
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
    // допускаем погрешность
    static const double error = 1.3;

    Quantity balance = OrderUtil::usingQuantity(side, symbol.baseAsset().getBalance(), symbol.quoteAsset().getBalance());
    Quantity cost = OrderUtil::usingQuantity(side, quantity, symbol.price(quantity));
    return balance > cost * error;
}
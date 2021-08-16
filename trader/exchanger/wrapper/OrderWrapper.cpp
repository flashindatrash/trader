#include "OrderWrapper.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/base/Symbol.hpp"

OrderWrapper* OrderWrapper::create()
{
    OrderWrapper* wrapper = new OrderWrapper();
    return wrapper;
}

void OrderWrapper::set(Order data) {
    _data = data;
}

const std::string& OrderWrapper::getId() const {
    return _data.id;
}

const OrderSide& OrderWrapper::side() const {
    return _data.side;
}

const double& OrderWrapper::quantity() const {
    return _data.quantity;
}

const Price OrderWrapper::getPrice() const {
    return _data.quoute_quantity / _data.quantity;
}

bool OrderRequest::isEnough() const {
    const Symbol& symbol = Exchanger().book()->getIdentifier();
    if (side == OrderSide::Buy)
        return symbol.quoteAsset().getBalance() >= symbol.getPrice(quantity);
    else if (side == OrderSide::Sell)
        return symbol.baseAsset().getBalance() > quantity;
    return false;
}

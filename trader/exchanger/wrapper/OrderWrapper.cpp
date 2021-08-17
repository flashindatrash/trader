#include "OrderWrapper.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/ExchangeWrapper.hpp"

OrderWrapper* OrderWrapper::create()
{
    OrderWrapper* wrapper = new OrderWrapper();
    return wrapper;
}

void OrderWrapper::set(Order data) {
    _data = data;
}

const Order::Id& OrderWrapper::getId() const {
    return _data.id;
}

const OrderSide& OrderWrapper::side() const {
    return _data.side;
}

const Quantity& OrderWrapper::quantity() const {
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

bool OrderRequest::canTrade() const {
    if (not isEnough())
        return false;

    const Symbol& symbol = Exchanger().book()->getIdentifier();

    const ExchangeWrapper* info = Exchanger().info(symbol);
    if (info == nullptr)
        return false;

    if (not info->hasOrderType(type))
        return false;

    /*
    const BinanceSymbolData& info = symbol.getInfo();

    // check minNotional for market
    if (info.minNotional.applyToMarket) {
        double avgPrice = symbol.getPrice();
        if (const PriceWrapper* history = Exchanger().price(symbol))
            avgPrice = history->getPriceAverage(info.minNotional.avgPriceMins * TraderTime::sMinute);

        if (avgPrice * quantity < info.minNotional.minNotional) {
            Logger::info("can't trade %f %s less minNotional", quantity, symbol.c_str());
            return BinanceOrderData();
        }
    }

    // check lot size
    if (info.lotSize.has) {
        if (quantity < info.lotSize.minQty) {
            Logger::info("can't trade %f quantity (min %f)", quantity, info.lotSize.minQty);
            return BinanceOrderData();
        } else if (quantity > info.lotSize.maxQty) {
            Logger::info("can't trade %f quantity (max %f)", quantity, info.lotSize.maxQty);
            return BinanceOrderData();
        }
        // todo
        //else if ((quantity - info.lotSize.minQty) % info.lotSize.stepSize != 0) {
        //    Logger::info("can't trade %f quantity (step size %f)\n", quantity, info.lotSize.stepSize);
        //    return BinanceOrderData();
        //}
    }
    */
    return true;
}

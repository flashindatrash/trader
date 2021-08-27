#include "OrderWrapper.hpp"

#include <utility>
#include "exchanger/base/Symbol.hpp"

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
    if (side == OrderSide::Buy)
        return symbol.quoteAsset().getBalance() >= symbol.getPrice(quantity);
    else if (side == OrderSide::Sell)
        return symbol.baseAsset().getBalance() > quantity;
    return false;
}

bool OrderRequest::canTrade() const {
    if (not isEnough())
        return false;

    /*
     *
    if (not info->hasOrderType(type))
        return false;

    const BinanceSymbolData& info = symbol.getInfo();

    // check minNotional for market
    if (info.minNotional.applyToMarket) {
        double avgPrice = symbol.getPrice();
        if (const PriceWrapper* history = Exchanger().price(symbol))
            avgPrice = history->getPriceAverage(info.minNotional.avgPriceMins * TraderTime::sMinute);

        if (avgPrice * base_quantity < info.minNotional.minNotional) {
            Logger::info("can't trade %f %s less minNotional", base_quantity, symbol.c_str());
            return BinanceOrderData();
        }
    }

    // check lot size
    if (info.lotSize.has) {
        if (base_quantity < info.lotSize.minQty) {
            Logger::info("can't trade %f base_quantity (min %f)", base_quantity, info.lotSize.minQty);
            return BinanceOrderData();
        } else if (base_quantity > info.lotSize.maxQty) {
            Logger::info("can't trade %f base_quantity (max %f)", base_quantity, info.lotSize.maxQty);
            return BinanceOrderData();
        }
        // todo
        //else if ((baseQuantity - info.lotSize.minQty) % info.lotSize.stepSize != 0) {
        //    Logger::info("can't trade %f baseQuantity (step size %f)\n", baseQuantity, info.lotSize.stepSize);
        //    return BinanceOrderData();
        //}
    }
    */
    return true;
}

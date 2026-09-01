//
// Created by Вадим Проскурин on 07.08.2022.
//

#include "OrderCreator.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Position.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

bool OrderCreator::create(OrderRequest& request, Position& result, bool release) {
    if (request.side == OrderSide::Invalid)
        return false;

    if (not release) {
        const Asset& asset = OrderUtil::usedAsset(request.side, request.symbol);
        if (request.mask(WalletRequest::CheckBalance)) {
            if (asset.balance() < request.required())
                return false;
        }

        // price is custom and known
        Decimal price = result.quoteQuantity();
        if (price == Decimal::Zero)
            price = request.symbol.price(request.side);

        result.setSymbol(request.symbol);
        result.setSide(request.side);
        result.setBaseQuantity(Exchanger().roundQuantity(request.quantity, request.symbol));
        result.setQuoteQuantity(result.baseQuantity() * price);
        result.operate();
        return true;
    }

    // создание заказа
    const OrderWrapper* order = Exchanger().createOrder(request);
    if (order == nullptr)
        return false;

    result.copy(*order);
    return true;
}

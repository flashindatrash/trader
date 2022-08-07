//
// Created by Вадим Проскурин on 07.08.2022.
//

#include "OrderController.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Position.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

bool OrderController::create(OrderRequest& request, Position& result, bool release) {
    if (request.side == OrderSide::Invalid)
        return false;

    if (not release) {
        const Asset& asset = OrderUtil::usedAsset(request.side, request.symbol);
        if (request.mask(WalletRequest::CheckBalance)) {
            Decimal balance = asset.balance();
            if (request.mask(WalletRequest::RedeemSavings))
                balance += Asset("LD" + asset.id()).balance();

            if (balance < request.required())
                return false;
        }

        // price is custom and known
        Decimal price = result.quoteQuantity();
        if (price == 0)
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
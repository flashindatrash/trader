//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Status.hpp"
#include "Logger.hpp"
#include "exchanger/base/OrderBase.hpp"
#include "util/NumberUtil.hpp"

NS_USE

Status* Status::create(const Symbol& symbol) {
    auto* status = new Status(symbol);
    return status;
}

Status::Status(const Symbol& symbol)
    : _symbol(symbol)
{
}

void Status::update() {
    double baseBalance = _symbol.baseAsset().getBalance();
    double quoteBalance = _symbol.quoteAsset().getBalance();

    std::string formatBaseBalance = "%." + std::to_string(util::zeros_after_dot(baseBalance) + 1) + "f";
    std::string formatQuoteBalance = "%." + std::to_string(util::zeros_after_dot(quoteBalance) + 1) + "f";

    std::string format = "%s (" + formatBaseBalance + ") - %s (" + formatQuoteBalance + ")";
    Logger::title(format.c_str(),
                  _symbol.baseAsset().c_str(),
                  baseBalance,
                  _symbol.quoteAsset().c_str(),
                  quoteBalance);
}

void Status::addOrder(const OrderBase& order, std::string type) {
    Logger::info("%s %s position %f for %f", type.c_str(), order.side() == OrderSide::Buy ? "Buy" : "Sell", order.baseQuantity(), order.price());
}
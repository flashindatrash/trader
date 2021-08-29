//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Status.hpp"
#include "Logger.hpp"
#include "exchanger/base/OrderBase.hpp"
#include <utility>

NS_USE

void Status::setTitle(const Symbol& symbol) {
    Logger::title("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

void Status::printOrder(const OrderBase& order, const std::string& type) {
    Logger::info("%s %s %f for %f", type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price());
}

void Status::addProfit(Quantity profit) {
    Logger::info("%sprofit: %f%s", GREEN, profit, RESET);
}

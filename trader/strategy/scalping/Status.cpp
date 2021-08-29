//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Status.hpp"

#include <utility>
#include "Logger.hpp"
#include "Context.hpp"
#include "exchanger/base/OrderBase.hpp"
#include "util/NumberUtil.hpp"

NS_USE

Status* Status::create(const Symbol& symbol) {
    auto* status = new Status(symbol);
    return status;
}

Status::Status(Symbol symbol)
    : _symbol(std::move(symbol))
{
}

void Status::update(const Context& context, Algorithm::Result close, Algorithm::Result open) {
    double baseBalance = _symbol.baseAsset().getBalance();
    double quoteBalance = _symbol.quoteAsset().getBalance();

    std::string formatBaseBalance = "%." + std::to_string(util::zeros_after_dot(baseBalance) + 1) + "f";
    std::string formatQuoteBalance = "%." + std::to_string(util::zeros_after_dot(quoteBalance) + 1) + "f";
    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(context.price()) + 3) + "f";

    std::string format = "%s (" + formatBaseBalance + ") - %s (" + formatQuoteBalance + ") " + formatPrice + " open(%s) close(%s)";
    Logger::title(format.c_str(),
                  _symbol.baseAsset().c_str(),
                  baseBalance,
                  _symbol.quoteAsset().c_str(),
                  quoteBalance,
                  context.price(),
                  result_str(open).c_str(),
                  result_str(close).c_str());
}

void Status::addOrder(const OrderBase& order, const std::string& type) {
    Logger::info("%s %s %f for %f", type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price());
}

void Status::addProfit(Quantity profit) {
    Logger::info("%sprofit: %f%s", GREEN, profit, RESET);
}

std::string Status::result_str(Algorithm::Result result) {
    switch (result) {
        case Algorithm::OK: return "OK";
        case Algorithm::INVALID: return "INVALID";
        case Algorithm::FAILED: return "FAILED";
        case Algorithm::NOT_ENOUGH: return "NOT ENOUGH";
        case Algorithm::CLOSE_NOT_EXISTS: return "NOT EXISTS";
        case Algorithm::CLOSE_NON_PROFITABLE: return "NON PROFITABLE";
        case Algorithm::OPEN_WAIT_PROFIT: return "WAIT PROFIT";
        case Algorithm::OPEN_LIMIT: return "LIMIT";
        case Algorithm::OPEN_PROFIT_SUPPLY: return "PROFIT SUPPLY";
    }
    return "";
}

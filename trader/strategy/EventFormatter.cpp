//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "EventFormatter.hpp"
#include "Position.hpp"
#include "Context.hpp"
#include "Report.hpp"
#include "util/MathUtil.hpp"
#include "util/StringUtil.hpp"
#include <utility>

NS_USE

std::string EventFormatter::title(const Symbol& symbol) {
    return util::format("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

std::string EventFormatter::update(const Position& position, const Context& context) {
    Price current_price = context.price(position.revert());
    Price position_price = position.price();

    Quantity profit = position.profit(current_price);
    std::string profitFormat;
    profitFormat.append("%." + std::to_string(util::zeros_after_dot(profit) + 2) + "f ");
    profitFormat.append(position.symbol().quoteAsset());

    Change change = position.change(current_price) * 100.0;
    std::string changeFormat = "%.2f%%";

    std::string positionFormat;
    positionFormat.append(position.side() == OrderSide::Buy ? "long" : "short");
    positionFormat.append(": %." + std::to_string(util::zeros_after_dot(position_price) + 2) + "f");
    positionFormat.append(" > %." + std::to_string(util::zeros_after_dot(current_price) + 2) + "f");

    Quantity balance = position.symbol().balance(Asset::USDT);
    std::string balanceFormat = "balance: %." + std::to_string(util::zeros_after_dot(balance) + 2) + "f " + Asset::USDT.id();

    std::string format = profitFormat + " " + changeFormat + " [" + positionFormat + "] [" + balanceFormat + "]";
    return util::format(format.c_str(), profit, change, position_price, current_price, balance);
}

std::string EventFormatter::order(const OrderBase& order) {
    std::string formatQuantity = "%." + std::to_string(util::zeros_after_dot(order.baseQuantity()) + 2) + "f";
    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(order.price()) + 2) + "f";

    std::string format = "%s " + formatQuantity + " %s for " + formatPrice + " %s";
    return util::format(format.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.symbol().baseAsset().c_str(), order.price(), order.symbol().quoteAsset().c_str());
}

std::string EventFormatter::profit(const Report& report, const Asset& asset) {
    std::string formatProfit = "%." + std::to_string(util::zeros_after_dot(report.profit) + 2) + "f";

    if (report.profit > 0)
        formatProfit = "+ " + formatProfit;

    std::string format = formatProfit + " %s (%0.2f%%)";
    return util::format(format.c_str(), report.profit, asset.c_str(), report.change * 100);
}

std::string EventFormatter::report(const Report& report, const Symbol& symbol) {
    return util::format("Report:\n\tSuccess: %0.0f%% (%d of %d positions)\n\tChange: %0.2f%%\n\tProfit: %f %s\n\tUse %s: %f\n\tUse %s: %f\n\tBalance %s: %f\n\tBalance %s: %f",
                 (double)report.success / (double)report.positions * 100.0, report.success, report.positions,
                 report.change * 100.0,
                 report.profit, symbol.quoteAsset().c_str(),
                 symbol.baseAsset().c_str(), report.use_base,
                 symbol.quoteAsset().c_str(), report.use_quote,
                 symbol.baseAsset().c_str(), symbol.baseAsset().balance(),
                 symbol.quoteAsset().c_str(), symbol.quoteAsset().balance());
}
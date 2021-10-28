//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Terminal.hpp"
#include "Logger.hpp"
#include "Position.hpp"
#include "Context.hpp"
#include "Report.hpp"
#include "util/MathUtil.hpp"
#include <utility>

NS_USE

void Terminal::setTitle(const Symbol& symbol) {
    Logger::title("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

void Terminal::update(Position& position, const Symbol& symbol, const Context& context) {
    Price current_price = context.price(position.revert());
    Price position_price = position.price();

    Quantity profit = position.profit(current_price);
    std::string profitFormat;
    profitFormat.append(profit < 0 ? RED : GREEN);
    profitFormat.append("%." + std::to_string(util::zeros_after_dot(profit) + 2) + "f ");
    profitFormat.append(position.symbol().quoteAsset());
    profitFormat.append(RESET);

    Change change = position.change(current_price) * 100.0;
    std::string changeFormat = "%.2f%%";

    std::string positionFormat;
    positionFormat.append(position.side() == OrderSide::Buy ? "long" : "short");
    positionFormat.append(": %." + std::to_string(util::zeros_after_dot(position_price) + 2) + "f");
    positionFormat.append(" > %." + std::to_string(util::zeros_after_dot(current_price) + 2) + "f");

    Quantity balance = symbol.balance(Asset::USDT);
    std::string balanceFormat = "balance: %." + std::to_string(util::zeros_after_dot(balance) + 2) + "f " + Asset::USDT.id();

    std::string format = profitFormat + " " + changeFormat + " [" + positionFormat + "] [" + balanceFormat + "]";
    Logger::status(format.c_str(), profit, change, position_price, current_price, balance);
}

void Terminal::printOrder(const OrderBase& order, const std::string& type) {
    std::string formatQuantity = "%." + std::to_string(util::zeros_after_dot(order.baseQuantity()) + 2) + "f";
    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(order.price()) + 2) + "f";

    std::string format = "%s %s\t" + formatQuantity + " for\t" + formatPrice;
    Logger::info(format.c_str(), type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price());
}

void Terminal::printProfit(const Report& report, const Asset& asset) {
    std::string formatProfit = "%." + std::to_string(util::zeros_after_dot(report.profit) + 2) + "f";

    if (report.profit > 0)
        formatProfit = "+ " + formatProfit;

    std::string format = "%s" + formatProfit + " %s (%0.2f%%)%s";
    Logger::info(format.c_str(), report.profit > 0 ? GREEN : RED, report.profit, asset.c_str(), report.change * 100, RESET);
}

void Terminal::printReport(const Report& report, const Symbol& symbol) {
    Logger::info("Report:\n\t%sChange: %0.2f%%\n\tProfit: %f %s\n\tUse %s: %f\n\tUse %s: %f\n\tBalance %s: %f\n\tBalance %s: %f%s",
                 YELLOW,
                 report.change * 100.0,
                 report.profit, symbol.quoteAsset().c_str(),
                 symbol.baseAsset().c_str(), report.use_base,
                 symbol.quoteAsset().c_str(), report.use_quote,
                 symbol.baseAsset().c_str(), symbol.baseAsset().balance(),
                 symbol.quoteAsset().c_str(), symbol.quoteAsset().balance(),
                 RESET);
}
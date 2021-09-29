//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Terminal.hpp"
#include "Logger.hpp"
#include "Position.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Report.hpp"
#include "exchanger/base/OrderBase.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "util/NumberUtil.hpp"
#include <utility>

NS_USE

void Terminal::setTitle(const Symbol& symbol) {
    Logger::title("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

void Terminal::update(Position& position, const Settings& settings, const Context& context) {
    if (settings.isBackTest() || not position.has())
        return;

    Price current = context.price();

    Quantity profit = position.profit(current);
    std::string positionFormat;
    positionFormat.append(profit < 0 ? RED : GREEN);
    positionFormat.append("%." + std::to_string(util::zeros_after_dot(profit) + 2) + "f ");
    positionFormat.append(position.symbol().quoteAsset());
    positionFormat.append(RESET);

    Quantity balance = settings.symbol.balance(Asset::USDT);
    std::string balanceFormat = "[balance: %." + std::to_string(util::zeros_after_dot(balance) + 2) + "f " + Asset::USDT.id() + "]";

    std::string format = positionFormat + " " + balanceFormat;
    Logger::status(format.c_str(), profit, balance);
}

void Terminal::printOrder(const OrderBase& order, const std::string& type) {
    std::string formatQuantity = "%." + std::to_string(util::zeros_after_dot(order.baseQuantity()) + 2) + "f";
    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(order.price()) + 2) + "f";

    std::string format = "%s %s\t" + formatQuantity + " for " + formatPrice;
    Logger::info(format.c_str(), type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price());
}

void Terminal::printProfit(Quantity profit, const Asset& asset) {
    std::string formatProfit = "%." + std::to_string(util::zeros_after_dot(profit) + 2) + "f";

    if (profit > 0)
        formatProfit = "+ " + formatProfit;

    std::string format = "%s" + formatProfit + " %s%s";
    Logger::info(format.c_str(), profit > 0 ? GREEN : RED, profit, asset.c_str(), RESET);
}

void Terminal::printReport(const Report& report, const Symbol& symbol) {
    Logger::info("Report:\n\t%sProfit: %f %s\n\tUse %s: %f\n\tUse %s: %f%s",
                 YELLOW,
                 report.profit, symbol.quoteAsset().c_str(),
                 symbol.baseAsset().c_str(), report.use_base,
                 symbol.quoteAsset().c_str(), report.use_quote,
                 RESET);
}
//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Terminal.hpp"
#include "Logger.hpp"
#include "Positions.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "exchanger/base/OrderBase.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "util/NumberUtil.hpp"
#include <utility>
#include <vector>

NS_USE

void Terminal::setTitle(const Symbol& symbol) {
    Logger::title("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

void Terminal::update(Positions& positions, const Settings& settings, const Context& context) {
    if (positions.size() == 0)
        return;

    std::sort(positions.begin(), positions.end(), Compares::priceMax);

    std::string timeline;
    bool current_embeded = false;
    for (const Position& position : positions) {
        if (not current_embeded && context.price(position.revert()) < position.price()) {
            timeline.append("|");
            current_embeded = true;
        }
        if (position.side() == OrderSide::Buy)
            timeline.append("+");
        else if (position.side() == OrderSide::Sell)
            timeline.append("-");
    }

    if (not current_embeded)
        timeline.append("|");

    std::string close;
    const auto profitable = positions.compare_if(Predicates::closable, Compares::profitable);
    if (profitable != positions.cend()) {
        double percent = profitable->distance() / (profitable->price() * settings.take_profit);
        int percent_int = (int)(percent * 100.0);
        close.append("[");
        close.append(percent_int < 0 ? RED : GREEN);
        close.append(std::to_string(percent_int) + "%");
        close.append(RESET);
        close.append("]");
    }

    Quantity balance = settings.symbol.balance(Asset::USDT);
    std::string balanceFormat = "%." + std::to_string(util::zeros_after_dot(balance) + 2) + "f " + Asset::USDT.id();

    std::string format = "%s %s " + balanceFormat + " (%f / %f) (%f / %f / %f)";
    Logger::status(format.c_str(), timeline.c_str(), close.c_str(), balance,
                   context.risk(OrderSide::Buy), context.risk(OrderSide::Sell),
                   context.price(OrderSide::Invalid), context.price(OrderSide::Buy), context.price(OrderSide::Sell));
}

void Terminal::printOrder(const OrderBase& order, const std::string& type) {
    std::string formatQuantity = "%." + std::to_string(util::zeros_after_dot(order.baseQuantity()) + 2) + "f";
    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(order.price()) + 2) + "f";

    std::string format = "%s %s\t" + formatQuantity + " for " + formatPrice;
    Logger::info(format.c_str(), type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price());
}

void Terminal::printProfit(Quantity profit, Quantity sum) {
    std::string formatProfit = "%." + std::to_string(util::zeros_after_dot(profit) + 2) + "f";
    std::string formatSum = "(%." + std::to_string(util::zeros_after_dot(sum) + 2) + "f)";

    if (profit > 0)
        formatProfit = "+ " + formatProfit;

    std::string format = "%s" + formatProfit + "\t" + formatSum + "%s";
    Logger::info(format.c_str(), profit > 0 ? GREEN : RED, profit, sum, RESET);
}
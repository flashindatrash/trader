//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Status.hpp"
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

void Status::setTitle(const Symbol& symbol) {
    Logger::title("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

void Status::update(Positions &positions, const Settings& settings, const Context& context) {
    if (positions.size() == 0)
        return;

    std::sort(positions.begin(), positions.end(), Compares::max);

    std::string timeline;
    bool current_embeded = false;
    for (const Position& position : positions) {
        Price price = position.price();
        OrderSide side = position.side();

        if (not current_embeded && context.price() < price) {
            timeline.append("|");
            current_embeded = true;
        }
        if (side == OrderSide::Buy)
            timeline.append("+");
        else if (side == OrderSide::Sell)
            timeline.append("-");
    }

    if (not current_embeded)
        timeline.append("|");

    std::string close;
    const auto profitable = positions.compare_if(Predicates::closable(settings.symbol),
                                                 Compares::profitable(context.price()));
    if (profitable != positions.cend()) {
        double percent = profitable->distance(context.price()) / (profitable->price() * settings.close_position_percent);
        int percent_int = (int)(percent * 100.0);
        close.append("[");
        close.append(percent_int < 0 ? RED : GREEN);
        close.append(std::to_string(percent_int) + "%");
        close.append(RESET);
        close.append("]");
    }

    std::string format = "%s %s";
    Logger::status(format.c_str(), timeline.c_str(), close.c_str());
}

void Status::printOrder(const OrderBase& order, const std::string& type) {
    std::string formatQuantity = "%." + std::to_string(util::zeros_after_dot(order.baseQuantity()) + 2) + "f";
    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(order.price()) + 2) + "f";

    std::string format = "%s %s\t" + formatQuantity + " for " + formatPrice + " (%s)";
    Logger::info(format.c_str(), type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price(), order.id().c_str());
}

void Status::printProfit(Quantity profit, Quantity profits, Quantity losses) {
    Quantity PNL = profits + losses;

    std::string formatProfit = "+%." + std::to_string(util::zeros_after_dot(profit) + 2) + "f";
    std::string formatPNL = "(PNL %." + std::to_string(util::zeros_after_dot(PNL) + 2) + "f)";

    std::string format = "%s" + formatProfit + " " + formatPNL + "%s";
    Logger::info(format.c_str(), GREEN, profit, PNL, RESET);
}

void Status::printBalance(const Symbol &symbol) {
    std::string baseAsset = "%." + std::to_string(util::zeros_after_dot(symbol.baseAsset().getBalance()) + 2) + "f";
    std::string quoteAsset = "%." + std::to_string(util::zeros_after_dot(symbol.quoteAsset().getBalance()) + 2) + "f";

    std::string format = baseAsset + " %s / " + quoteAsset + " %s";
    Logger::info(format.c_str(), symbol.baseAsset().getBalance(), symbol.baseAsset().c_str(), symbol.quoteAsset().getBalance(), symbol.quoteAsset().c_str());
}
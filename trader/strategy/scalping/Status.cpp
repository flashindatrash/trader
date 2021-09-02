//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Status.hpp"
#include "Logger.hpp"
#include "Positions.hpp"
#include "Settings.hpp"
#include "exchanger/base/OrderBase.hpp"
#include <utility>
#include <vector>

NS_USE

void Status::setTitle(const Symbol& symbol, Price current) {
    Logger::title("%s - %s %f", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str(), current);
}

void Status::printTimeline(Positions &positions, Price current, const Settings& settings) {
    if (positions.size() == 0)
        return;

    std::sort(positions.begin(), positions.end(), Compares::max);

    std::string timeline;
    bool current_embeded = false;
    for (const Position& position : positions) {
        Price price = position.price();
        OrderSide side = position.side();

        if (not current_embeded && current < price) {
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

    const auto profitable = positions.compare_if(Predicates::closable(settings.symbol), Compares::distance(current));
    if (profitable != positions.cend()) {
        double percent = profitable->distance(current) / (current * settings.close_position_percent * 100.0);
        timeline.append(" [" + std::to_string((int)percent) + "%]");
    }

    Logger::status(timeline.c_str());
}

void Status::printOrder(const OrderBase& order, const std::string& type) {
    Logger::info("%s %s %f for %f", type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price());
}

void Status::addProfit(Quantity profit, const Symbol& symbol) {
    Logger::info("%sprofit: %f (%f %s, %f %s)%s", GREEN, profit, symbol.baseAsset().getBalance(), symbol.baseAsset().c_str(), symbol.quoteAsset().getBalance(), symbol.quoteAsset().c_str(), RESET);
}

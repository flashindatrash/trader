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
#include <utility>
#include <vector>

NS_USE

void Status::setTitle(const Symbol& symbol) {
    Logger::title("%s - %s %f", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

void Status::update(Positions &positions, const Settings& settings, const Context& context) {
    if (positions.size() == 0)
        return;

    std::sort(positions.begin(), positions.end(), Compares::max);

    std::string line;
    bool current_embeded = false;
    for (const Position& position : positions) {
        Price price = position.price();
        OrderSide side = position.side();

        if (not current_embeded && context.price() < price) {
            line.append("|");
            current_embeded = true;
        }
        if (side == OrderSide::Buy)
            line.append("+");
        else if (side == OrderSide::Sell)
            line.append("-");
    }

    if (not current_embeded)
        line.append("|");

    const auto profitable = positions.compare_if(Predicates::closable(settings.symbol), Compares::distance(context.price()));
    if (profitable != positions.cend()) {
        double percent = profitable->distance(context.price()) / (context.price() * settings.close_position_percent);
        line.append(" [" + std::to_string((int)(percent * 100.0)) + "%]");
    }

    if (context.candlestick->isBullish()) {
        line.append(GREEN);
        line.append("↑");
    } else if (context.candlestick->isBearish()) {
        line.append(RED);
        line.append("↓");
    }

    Logger::status(line.c_str());
}

void Status::printOrder(const OrderBase& order, const std::string& type) {
    Logger::info("%s %s\t%f for %f", type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price());
}

void Status::addProfit(Quantity profit, const Symbol& symbol) {
    Logger::info("%= %f (%f %s, %f %s)%s", GREEN, profit, symbol.baseAsset().getBalance(), symbol.baseAsset().c_str(), symbol.quoteAsset().getBalance(), symbol.quoteAsset().c_str(), RESET);
}

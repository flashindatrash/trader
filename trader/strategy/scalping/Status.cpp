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
    const auto profitable = positions.compare_if(Predicates::closable(settings.symbol), Compares::distance(context.price()));
    if (profitable != positions.cend()) {
        double percent = profitable->distance(context.price()) / (context.price() * settings.close_position_percent);
        close = "[" + std::to_string((int)(percent * 100.0)) + "%]";
    }

    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(context.price()) + 3) + "f";
    std::string price = formatPrice;
    if (context.candlestick->isBullish()) {
        price.append(GREEN);
        price.append("↑" + formatPrice);
        price.append(RESET);
    } else if (context.candlestick->isBearish()) {
        price.append(RED);
        price.append("↓" + formatPrice);
        price.append(RESET);
    }

    std::string format = "%s %s " + price;
    Logger::status(format.c_str(), timeline.c_str(), close.c_str(), context.price());
}

void Status::printOrder(const OrderBase& order, const std::string& type) {
    Logger::info("%s %s\t%f for %f", type.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.price());
}

void Status::addProfit(Quantity profit, const Symbol& symbol) {
    Logger::info("%= %f (%f %s, %f %s)%s", GREEN, profit, symbol.baseAsset().getBalance(), symbol.baseAsset().c_str(), symbol.quoteAsset().getBalance(), symbol.quoteAsset().c_str(), RESET);
}

//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Formatter.hpp"
#include "Position.hpp"
#include "Context.hpp"
#include "Report.hpp"
#include "Settings.hpp"
#include "Logger.hpp"
#include "exchanger/Exchanger.hpp"
#include "util/MathUtil.hpp"
#include "util/StringUtil.hpp"
#include <utility>

NS_USE

static const char* sImportantBegin = "[*]";
static const char* sImportantEnd = "[/*]";

Formatter Formatter::title(const Symbol& symbol) {
    return util::format("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

Formatter Formatter::update(const Position& position, const Context& context, const Settings& settings) {
    Price current_price = context.price(position.revert());
    Price position_price = position.price();
    Quantity profit = position.profit(current_price);

    std::string result;
    result.append(position.side() == OrderSide::Buy ? "long" : "short");
    result.append(": " + asset(position_price) + " > " + asset(current_price));
    result.append(util::format(" change %.2f%%", position.change(current_price) * 100.0));
    result.append(sImportantBegin);
    result.append(" " + asset(profit, position.symbol().quoteAsset()));
    result.append(sImportantEnd);
    return result;
}

Formatter Formatter::order(const OrderBase& order) {
    std::string side = order.side() == OrderSide::Buy ? "buy" : "sell";
    std::string quantity = asset(order.baseQuantity(), order.symbol().baseAsset());
    std::string price = asset(order.price(), order.symbol().quoteAsset());
    return util::format("%s %s for %s", side.c_str(), quantity.c_str(), price.c_str());
}

Formatter Formatter::profit(const Report& report, const Symbol& symbol) {
    std::string result = "profit";
    if (std::abs(report.earn_base) > std::numeric_limits<double>::epsilon())
        result += " " + asset(report.earn_base, symbol.baseAsset(), true);
    if (std::abs(report.earn_quote) > std::numeric_limits<double>::epsilon())
        result += " " + asset(report.earn_quote, symbol.quoteAsset(), true);
    return result;
}

Formatter Formatter::report(const Report& report, const Symbol& symbol) {
    return util::format("Report:\n\tSuccess: %0.0f%% (%d of %d positions)\n\tProfit: %f (APY: %.1f%%)\n\tEarn %s: %f\n\tEarn %s: %f\n\tVolume %s: %f\n\tVolume %s: %f\n\tUse %s: %f\n\tUse %s: %f",
                 (double)report.success / (double)report.positions * 100.0, report.success, report.positions,
                 report.profit, report.apy() * 100.0,
                 symbol.baseAsset().c_str(), report.earn_base,
                 symbol.quoteAsset().c_str(), report.earn_quote,
                 symbol.baseAsset().c_str(), report.volume_base,
                 symbol.quoteAsset().c_str(), report.volume_quote,
                 symbol.baseAsset().c_str(), report.use_base,
                 symbol.quoteAsset().c_str(), report.use_quote);
}

std::string Formatter::asset(Quantity quantity, const Asset& asset/* = Asset::Empty*/, bool change/* = false*/) {
    std::string result;
    if (change) result.append(sImportantBegin);
    if (change && quantity > 0) result.append("+");
    std::string format = "%." + std::to_string(util::zeros_after_dot(quantity) + 2) + "f";
    result.append(util::format(format.c_str(), quantity));
    if (change) result.append(sImportantEnd);
    if (not asset.id().empty()) result.append(util::format(" %s", asset.c_str()));
    return result;
}

Formatter::Formatter(std::string value)
    : _text(std::move(value))
{
}

std::string Formatter::terminal() const {
    std::string result = _text;
    replace(result, sImportantBegin, GREEN);
    replace(result, sImportantEnd, RESET);
    return result;
}

std::string Formatter::html() const {
    std::string result = _text;
    replace(result, sImportantBegin, "<u>");
    replace(result, sImportantEnd, "</u>");
    return result;
}

void Formatter::replace(std::string& text, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = text.find(from, start_pos)) != std::string::npos) {
        text.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

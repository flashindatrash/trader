//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "Formatter.hpp"
#include "Position.hpp"
#include "Statistics.hpp"
#include "Context.hpp"
#include "Report.hpp"
#include "Logger.hpp"
#include "util/MathUtil.hpp"
#include "util/StringUtil.hpp"
#include <utility>

NS_USE

static const char* sImportantBegin = "[*]";
static const char* sImportantEnd = "[/*]";

Formatter Formatter::title(const Symbol& symbol) {
    return util::format("%s - %s", symbol.baseAsset().c_str(), symbol.quoteAsset().c_str());
}

Formatter Formatter::update(const Position& position, const Context& context) {
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

    std::string format = profitFormat + " " + sImportantBegin + changeFormat + sImportantEnd + " [" + positionFormat + "]";
    return util::format(format.c_str(), profit, change, position_price, current_price);
}

Formatter Formatter::order(const OrderBase& order) {
    std::string formatQuantity = "%." + std::to_string(util::zeros_after_dot(order.baseQuantity()) + 2) + "f";
    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(order.price()) + 2) + "f";

    std::string format = "%s " + formatQuantity + " %s for " + formatPrice + " %s";
    return util::format(format.c_str(), order.side() == OrderSide::Buy ? "buy" : "sell", order.baseQuantity(), order.symbol().baseAsset().c_str(), order.price(), order.symbol().quoteAsset().c_str());
}

Formatter Formatter::profit(const Report& report, const Symbol& symbol) {
    std::string formatEarnBase = "%." + std::to_string(util::zeros_after_dot(report.earn_base) + 2) + "f";
    if (report.earn_base > 0)
        formatEarnBase = "+" + formatEarnBase;

    std::string formatEarnQuote = "%." + std::to_string(util::zeros_after_dot(report.earn_quote) + 2) + "f";
    if (report.earn_quote > 0)
        formatEarnQuote = "+" + formatEarnQuote;

    std::string format = sImportantBegin + formatEarnBase + sImportantEnd + " %s " + sImportantBegin + formatEarnQuote + sImportantEnd + " %s (%0.2f%%)";
    return util::format(format.c_str(), report.earn_base, symbol.baseAsset().c_str(), report.earn_quote, symbol.quoteAsset().c_str(), report.change * 100);
}

Formatter Formatter::report(const Report& report, const Symbol& symbol) {
    return util::format("Report:\n\tSuccess: %0.0f%% (%d of %d positions)\n\tChange: %0.2f%%\n\tProfit: %f %s\n\tEarn %s: %f\n\tEarn %s: %f\n\tUse %s: %f\n\tUse %s: %f\n\tBalance %s: %f\n\tBalance %s: %f",
                 (double)report.success / (double)report.positions * 100.0, report.success, report.positions,
                 report.change * 100.0,
                 report.profit, symbol.quoteAsset().c_str(),
                 symbol.baseAsset().c_str(), report.earn_base,
                 symbol.quoteAsset().c_str(), report.earn_quote,
                 symbol.baseAsset().c_str(), report.use_base,
                 symbol.quoteAsset().c_str(), report.use_quote,
                 symbol.baseAsset().c_str(), symbol.baseAsset().balance(),
                 symbol.quoteAsset().c_str(), symbol.quoteAsset().balance());
}

Formatter Formatter::stats(const Statistics& statistics, const Symbol& symbol) {
    return util::format("Stats: %f %s, %f %s (%f)",
                        statistics.earnBase(), symbol.baseAsset().c_str(),
                        statistics.earnQuote(), symbol.quoteAsset().c_str(),
                        statistics.profit());
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
//
// Created by Вадим Проскурин on 29.08.2021.
//

#include "processor/Formatter.hpp"
#include "Context.hpp"
#include "core/Logger.hpp"
#include "base/Settings.hpp"
#include "exchanger/base/Position.hpp"
#include "exchanger/base/Report.hpp"
#include <utility>
#include <limits>

using namespace trader;

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
    if (report.earn_base != Decimal::Zero)
        result += " " + asset(report.earn_base, symbol.baseAsset(), true);
    if (report.earn_quote != Decimal::Zero)
        result += " " + asset(report.earn_quote, symbol.quoteAsset(), true);
    return result;
}

Formatter Formatter::report(const Report& report, const Symbol& symbol) {
    return util::format("Report:\n\tSuccess: %0.0f%% (%d of %d positions)\n\tProfit: %s (APY: %.1f%%)\n\tEarn %s: %s\n\tEarn %s: %s\n\tVolume %s: %s\n\tVolume %s: %s\n\tUse %s: %s\n\tUse %s: %s",
                 (double)report.success / (double)report.positions * 100.0, report.success, report.positions,
                 report.profit.c_str(), report.apy() * 100.0,
                 symbol.baseAsset().c_str(), report.earn_base.c_str(),
                 symbol.quoteAsset().c_str(), report.earn_quote.c_str(),
                 symbol.baseAsset().c_str(), report.volume_base.c_str(),
                 symbol.quoteAsset().c_str(), report.volume_quote.c_str(),
                 symbol.baseAsset().c_str(), report.use_base.c_str(),
                 symbol.quoteAsset().c_str(), report.use_quote.c_str());
}

std::string Formatter::asset(Quantity quantity, const Asset& asset/* = Asset::Empty*/, bool change/* = false*/) {
    std::string result;
    if (change) result.append(sImportantBegin);
    if (change && quantity > Decimal::Zero) result.append("+");
    result.append(quantity.c_str());
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

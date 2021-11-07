//
// Created by Вадим Проскурин on 30.09.2021.
//

#include "Report.hpp"
#include "Position.hpp"

NS_USE

Report::Report(const Position& open, const Position& close) {
    success = open.count() == 1 ? 1 : 0;
    profit = open.profit(close.price());
    earn_base = OrderUtil::distance(close.side(), open.baseQuantity(), close.baseQuantity());
    earn_quote = OrderUtil::distance(open.side(), open.quoteQuantity(), close.quoteQuantity());
    change = open.change(close.price());
    use_base = open.baseQuantity();
    use_quote = open.quoteQuantity();
}

void Report::add(const Report& report) {
    ++positions;
    success += report.success;
    profit += report.profit;
    earn_quote += report.earn_quote;
    earn_base += report.earn_base;
    change += report.change;
    use_base = std::max(use_base, report.use_base);
    use_quote = std::max(use_quote, report.use_quote);
}
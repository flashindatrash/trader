//
// Created by Вадим Проскурин on 30.09.2021.
//

#include "Report.hpp"
#include "Position.hpp"

NS_USE

Report::Report(const Position& open, const Position& close) {
    success = open.count() == 1 ? 1 : 0;
    profit = open.profit(close.price());
    change = open.change(close.price());
    earn_base = OrderUtil::distance(close.side(), open.baseQuantity(), close.baseQuantity());
    earn_quote = OrderUtil::distance(open.side(), open.quoteQuantity(), close.quoteQuantity());
    use_base = OrderUtil::usedQuantity(open.side(), open.baseQuantity(), 0.0);
    use_quote = OrderUtil::usedQuantity(open.side(), 0.0, open.quoteQuantity());
}

void Report::add(const Report& report) {
    ++positions;
    success += report.success;
    profit += report.profit;
    change += report.change;
    earn_quote += report.earn_quote;
    earn_base += report.earn_base;
    use_base = std::max(use_base, report.use_base);
    use_quote = std::max(use_quote, report.use_quote);
}
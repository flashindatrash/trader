//
// Created by Вадим Проскурин on 30.09.2021.
//

#include "Report.hpp"
#include "Position.hpp"

NS_USE

Report::Report(const Position& open, const Position& close) {
    profit = open.profit(close.price());
    change = open.change(close.price());
    use_base = open.baseQuantity();
    use_quote = open.quoteQuantity();
}

void Report::add(const Report& report) {
    profit += report.profit;
    change += report.change;
    use_base = std::max(use_base, report.use_base);
    use_quote = std::max(use_quote, report.use_quote);
}
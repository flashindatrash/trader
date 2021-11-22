//
// Created by Вадим Проскурин on 30.09.2021.
//

#include "Report.hpp"
#include "Position.hpp"

NS_USE

Report::Report(const Position& open, const Position& close) {
    // берем пару из любой позиции
    const Symbol& symbol = open.symbol();
    // определяем свой депозит на момент закрытия
    Quantity deposit = open.symbol().balance(open.symbol().quoteAsset());

    success = open.count() == 1 ? 1 : 0;
    profit = open.profit(close.price());
    earn_base = OrderUtil::distance(close.side(), open.baseQuantity(), close.baseQuantity());
    earn_quote = OrderUtil::distance(open.side(), open.quoteQuantity(), close.quoteQuantity());
    use_base = OrderUtil::usedQuantity(open.side(), open.baseQuantity(), 0.0);
    use_quote = OrderUtil::usedQuantity(open.side(), 0.0, open.quoteQuantity());
    volume_base = open.baseQuantity() + close.baseQuantity();
    volume_quote = open.quoteQuantity() + close.quoteQuantity();
    change = profit / deposit;
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
    volume_base += report.volume_base;
    volume_quote += report.volume_quote;
}
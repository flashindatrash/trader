//
// Created by Вадим Проскурин on 30.09.2021.
//

#include "Report.hpp"
#include "Position.hpp"
#include "exchanger/base/Symbol.hpp"

Report::Report(const Position& open, const Position& close) {
    // берем пару из любой позиции
    const Symbol& symbol = open.symbol();

    success = open.count() == 1 ? 1 : 0;
    earn_base = OrderUtil::distance(close.side(), open.baseQuantity(), close.baseQuantity());
    earn_quote = OrderUtil::distance(open.side(), open.quoteQuantity(), close.quoteQuantity());
    use_base = OrderUtil::usedQuantity(open.side(), open.baseQuantity(), {});
    use_quote = OrderUtil::usedQuantity(open.side(), {}, open.quoteQuantity());
    use_total = symbol.baseAsset().convert(use_base) + symbol.quoteAsset().convert(use_quote);
    volume_base = open.baseQuantity() + close.baseQuantity();
    volume_quote = open.quoteQuantity() + close.quoteQuantity();

    // профит и процент от депо в USD
    profit = symbol.quoteAsset().convert(open.profit(close.price()));
}

void Report::add(const Report& report) {
    ++positions;
    success += report.success;
    profit += report.profit;
    earn_quote += report.earn_quote;
    earn_base += report.earn_base;
    use_base = std::max(use_base, report.use_base);
    use_quote = std::max(use_quote, report.use_quote);
    use_total = std::max(use_total, report.use_total);
    volume_base += report.volume_base;
    volume_quote += report.volume_quote;
}

Change Report::apy() const {
    return double(profit / use_total);
}
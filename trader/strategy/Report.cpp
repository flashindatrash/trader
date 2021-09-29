//
// Created by Вадим Проскурин on 30.09.2021.
//

#include "Report.hpp"
#include "Position.hpp"

NS_USE

Quantity Report::add(const Position& open, const Position& close) {
    Quantity added = open.profit(close.price()) - close.fee();
    profit += added;
    add_base += close.baseQuantity() - open.baseQuantity();
    add_quote += close.quoteQuantity() - open.quoteQuantity();
    use_base = std::max(use_base, close.baseQuantity());
    use_quote = std::max(use_quote, close.quoteQuantity());
    return added;
}
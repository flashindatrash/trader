#pragma once

class TradeSymbol;

namespace util {
    double get_min_quantity(const TradeSymbol& symbol);
    double ceil_quantity(const TradeSymbol& symbol, double quantity);
    double get_percent(double first, double second);
}


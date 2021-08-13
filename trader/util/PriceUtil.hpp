#pragma once

class Symbol;

namespace util {
    double get_min_quantity(const Symbol& symbol);
    double ceil_quantity(const Symbol& symbol, double quantity);
}


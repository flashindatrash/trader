#pragma once

class TradeSymbol;

namespace util {
    double get_min_quantity(const TradeSymbol& symbol);
    double ceil_quantity(const TradeSymbol& symbol, double quantity);
    double get_percent(double first, double second);

    // отношение баланса к среднему сумарному показателю
    // стремится к 1 - имеем больше половину баланса
    // стремится к 0 - отсутствуют средства
    // чем выше коэффициент, тем выше приоритет на сделки
    void calc_balance_rate(const TradeSymbol& symbol, float& base, float& quote);
}


#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinanceKlines.hpp"
#include "wrapper/KlineHistory.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "data/BinanceSymbolData.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/TraderManager.hpp"
#include "util/PriceUtil.hpp"

static float sMinRate = 0.003f;
static float sMaxRate = 0.01f;
static float sEqualRate = 0.02f;

TraderManager::TraderManager(OrderManager& orders)
    : BaseManager(orders)
{
    _interval = BinanceTime::sMinute * 10;
}

bool TraderManager::check(const TradeSymbol& symbol) {
    if (not BaseManager::check(symbol))
        return false;

    // устанавливаем стоимость покупки
    if (_min_quantity == 0.0) {
        _min_quantity = util::get_min_quantity(symbol) * 1.3;
        _min_quantity = util::ceil_quantity(symbol, _min_quantity);
        trace("trader min quantity: %f\n", _min_quantity);
    }

    const KlineHistory* history = SKlines().getHistory(symbol);
    if (history == nullptr)
        return false;

    const BinanceKlineData& kline = history->back();

    // посчитаем коэффициенты баланса
    float baseK = 0.0f;
    float quoteK = 0.0f;
    util::calc_balance_rate(symbol, baseK, quoteK);

    float change = util::get_percent(kline.priceOpen, kline.priceClose);
    float expected = sMinRate + (change > 0 ? baseK : quoteK) * (sMaxRate - sMinRate);
    if (std::abs(change) < expected)
        return false;

    std::string side = change > 0.0f ? "SELL" : "BUY";

    // не дублируем схожие транзакции
    if (hasEqualTransaction(side, symbol.getPrice()))
        return false;

    return _orders.create(symbol, side, _min_quantity, true);
}

bool TraderManager::hasEqualTransaction(const std::string& side, double price) const {
    for (const BinanceOrderData& order : _orders.getTransactions()) {
        // интересуют ордеры с одним типом
        if (order.side != side)
            continue;
        // проверяем разницу в цене на минимальный порог
        double change = util::get_percent(order.getPrice(), price);
        if (std::abs(change) < sEqualRate)
            return true;
    }
    return false;
}

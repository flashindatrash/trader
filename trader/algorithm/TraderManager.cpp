#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinanceKlines.hpp"
#include "wrapper/KlineHistory.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "data/BinanceSymbolData.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/TraderManager.hpp"
#include "algorithm/PriceAnalyzer.hpp"
#include "algorithm/DecisionMaker.hpp"
#include "util/PriceUtil.hpp"

// рост/падение в процентном соотношении, при котором стоит производить сделку
// диапозон между min/max выбирается в зависимости от текущего баланса
static float sMinRate = 0.005f;
static float sMaxRate = 0.04f;

// процентное соотношение цены для избегания открытия повторных схожих позиций
static float sEqualRate = 0.003f;

// мин объем валюты, с которым бот открывает новые заказы
// данное число умножается на минимальный разрешенный лот
static double sMinQuantity = 2.0;

TraderManager::TraderManager(OrderManager& orders)
    : BaseManager(orders, BinanceTime::sSecond * 30)
{
}

bool TraderManager::check(const TradeSymbol& symbol) {
    if (not BaseManager::check(symbol))
        return false;

    // устанавливаем стоимость покупки
    if (_min_quantity == 0.0) {
        double min = util::get_min_quantity(symbol);
        _min_quantity = util::ceil_quantity(symbol, min * sMinQuantity);
        trace("trader quantity: %f\n", _min_quantity);
    }

    const KlineHistory* history = SKlines().getHistory(symbol);
    if (history == nullptr)
        return false;

    PriceAnalyzer analyzer(*history);
    double change = analyzer.getStablePriceChange(_orders.getLastTime());

    DecisionMaker decision(symbol);
    if (not decision.make(change, sMinRate, sMaxRate, DecisionMaker::Balane))
        return false;


    // не дублируем схожие транзакции
    if (hasEqualTransaction(change, symbol.getPrice()))
        return false;

    return _orders.create(symbol, change, _min_quantity, nullptr);
}

bool TraderManager::hasEqualTransaction(const BinanceSideEnum& side, double price) const {
    for (const BinanceOrderData& order : _orders.getPositions()) {
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

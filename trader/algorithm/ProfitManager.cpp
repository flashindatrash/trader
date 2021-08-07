#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/Database.hpp"
#include "proxy/BinanceOrders.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "algorithm/ProfitManager.hpp"
#include "algorithm/OrderManager.hpp"
#include "util/PriceUtil.hpp"

static float sMinRate = 0.003f;
static float sMaxRate = 0.006f;

ProfitManager::ProfitManager(OrderManager& orders)
    : BaseManager(orders)
{
    _interval = BinanceTime::sSecond * 30;
}

bool ProfitManager::check(const TradeSymbol& symbol) {
    if (not BaseManager::check(symbol))
        return false;

    // находим ордер для закрытия
    const BinanceOrderData* transaction = findClosableOrder(symbol);
    if (transaction == nullptr)
        return false;

    // пробуем создать новый ордер
    const std::string& side = transaction->side == "BUY" ? "SELL" : "BUY";
    if (not _orders.create(symbol, side, transaction->quantity))
        return false;

    // закрываем старый ордер
    trace("close %s order for %f\n", transaction->side.c_str(), transaction->getPrice());
    _orders.close(*transaction);

    // сохраняем профит
    addProfitStats(std::abs(transaction->getPrice() - symbol.getPrice()) * transaction->quantity, symbol.quoteAsset());
    return true;
}

const BinanceOrderData* ProfitManager::findClosableOrder(const TradeSymbol &symbol) const {
    // посчитаем коэффициенты баланса
    float baseK = 0.0f;
    float quoteK = 0.0f;
    util::calc_balance_rate(symbol, baseK, quoteK);

    // найдем ордер, который стоит закрыть по более выгодному курсу
    const BinanceOrderData* transaction = nullptr;
    double best_change = 0.0;
    for (const BinanceOrderData& order : _orders.getTransactions()) {
        double change = util::get_percent(order.getPrice(), symbol.getPrice());
        double abs_change = std::abs(change);

        // если цена падает, но нам нужно продавать
        if (change < 0.0 && order.side == "BUY")
            continue;

        // если цена растет, но нам нужно покупать
        if (change > 0.0 && order.side == "SELL")
            continue;

        float expected = sMinRate + (change > 0 ? baseK : quoteK) * (sMaxRate - sMinRate);
        if (abs_change < expected)
            continue;

        if (abs_change < best_change)
            continue;

        // проверим, что достаточн средств для закрытия ордера
        if (not SOrders().isEnough(symbol, order.side == "SELL" ? "BUY" : "SELL", order.quantity))
            continue;

        transaction = &order;
        best_change = abs_change;
    }

    return transaction;
}

void ProfitManager::addProfitStats(double profit, const TradeAsset& asset) {
    std::string balance_key = "binance:stats:profit:" + asset;
    profit += DB().getAsDouble(balance_key);
    DB().set(balance_key, profit);
    trace("%sprofit update: +%.2f %s%s\n", GREEN, profit, asset.c_str(), RESET);
}

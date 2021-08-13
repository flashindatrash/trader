#include "Logger.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinanceTime.hpp"
#include "exchanger/wrapper/TradeSymbol.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DataManager.hpp"

OrderManager::OrderManager(const TradeSymbol& symbol)
{
    _orders = SOrders().getAllOrders(symbol);
    _last_time = DataManager::getLastOrderTime(symbol);

    // найдем все открытые позиции
    std::vector<std::string> keys = DataManager::getPositionIds();
    for (const BinanceOrderData& order : _orders) {
        std::string id = DataManager::sDbKeyOrder + order.clientOrderId;
        if (std::find(keys.begin(), keys.end(), id) == keys.end())
            continue;

        _positions.push_back(order);
    }

    printPositionsTimeline(symbol.getPrice());
}

bool OrderManager::create(const TradeSymbol& symbol, const BinanceSideEnum& side, double quantity, const BinanceOrderData* transaction) {
    // проверяем, что достаточно средств
    if (not SOrders().isEnough(symbol, side, quantity))
        return false;

    BinanceOrderData result = SOrders().createOrder(symbol, side, quantity);
    // неизвестная ошибка
    if (result.isEmpty())
        return false;

    // обновляем время даже если нам вернули сстатус REJECTED
    _last_time = STime().getCurrent();
    DataManager::setLastOrderTime(symbol, _last_time);

    // не удалось создать
    if (result.isRejected())
        return false;

    // сохраним историю
    _orders.push_back(result);

    // открыть/закрыть транзакцию
    if (transaction == nullptr) {
        trace("\a%s %f %s for %f\n", side.c_str(), quantity, symbol.baseAsset().c_str(), symbol.getPrice());
        DataManager::openPosition(result.clientOrderId);
        _positions.push_back(result);
    } else {
        trace("\a%s %f %s for %f (%s for %f)\n", side.c_str(), quantity, symbol.baseAsset().c_str(), symbol.getPrice(), transaction->side.c_str(), transaction->getPrice());
        double profit = std::abs(transaction->getPrice() - symbol.getPrice()) * transaction->quantity;
        printProfit(symbol, profit);
        std::string order_id = transaction->clientOrderId;
        DataManager::closePosition(order_id);
        _positions.erase(std::remove_if(_positions.begin(), _positions.end(), [order_id](BinanceOrderData t) { return t.clientOrderId == order_id; }));
    }

    printPositionsTimeline(symbol.getPrice());
    return true;
}

const std::vector<BinanceOrderData>& OrderManager::getOrders() const {
    return _orders;
}

const std::vector<BinanceOrderData>& OrderManager::getPositions() const {
    return _positions;
}

time_t OrderManager::getLastTime() const {
    return _last_time;
}

void OrderManager::printProfit(const TradeSymbol& symbol, double profit) {
    double profit_total = DataManager::addProfit(symbol.quoteAsset(), profit);
    double losses_total = 0.0;

    double current_price = symbol.getPrice();
    for (const BinanceOrderData& position : _positions) {
        double order_price = position.getPrice();

        bool sell_loss = position.side == BinanceSideEnum::Sell && current_price > order_price;
        bool buy_loss = position.side == BinanceSideEnum::Buy && current_price < order_price;

        if (sell_loss || buy_loss)
            losses_total += std::abs(current_price - order_price) * position.quantity;
    }

    trace("%sprofit update: +%.4f (total +%.4f / loss -%.4f) %s%s\n", GREEN, profit, profit_total, losses_total, symbol.quoteAsset().c_str(), RESET);
}

void OrderManager::printPositionsTimeline(double current) {
    if (_positions.empty())
        return;

    // отсортируем позиции
    std::sort(_positions.begin(), _positions.end(), [](const BinanceOrderData& l, const BinanceOrderData& r) {
        return l.getPrice() < r.getPrice();
    });

    bool current_embeded = false;
    for (const BinanceOrderData& position : _positions) {
        if (not current_embeded && current < position.getPrice()) {
            std::cout << "|";
            current_embeded = true;
        }
        if (position.side == BinanceSideEnum::Buy)
            std::cout << "+";
        else
            std::cout << "-";
    }

    if (not current_embeded)
        std::cout << "|";

    std::cout << std::endl;
}


#include "Logger.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinanceTime.hpp"
#include "wrapper/TradeSymbol.hpp"
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
        trace("- %s %f\n", order.side.c_str(), order.getPrice());
    }

    if (not _positions.empty()) {
        // отсортируем позиции
        std::sort(_positions.begin(), _positions.end(), [](const BinanceOrderData& l, const BinanceOrderData& r) {
            return l.getPrice() < r.getPrice();
        });

        double current_price = symbol.getPrice();
        bool current_embeded = false;
        for (const BinanceOrderData& position : _positions) {
            if (not current_embeded && current_price < position.getPrice()) {
                std::cout << "|";
                current_embeded = true;
            }
            if (position.side == BinanceSideEnum::Buy)
                std::cout << "+";
            else
                std::cout << "-";
        }
        std::cout << std::endl;
    }
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
        trace("\a%s %f %s for %f (prev %f)\n", side.c_str(), quantity, symbol.baseAsset().c_str(), symbol.getPrice(), transaction->getPrice());
        double profit = std::abs(transaction->getPrice() - symbol.getPrice()) * transaction->quantity;
        double profit_total = DataManager::addProfit(symbol.quoteAsset(), profit);
        trace("%sprofit update: +%.2f (total +%.2f) %s%s\n", GREEN, profit, profit_total, symbol.quoteAsset().c_str(), RESET);
        std::string order_id = transaction->clientOrderId;
        DataManager::closePosition(order_id);
        _positions.erase(std::remove_if(_positions.begin(), _positions.end(), [order_id](BinanceOrderData t) { return t.clientOrderId == order_id; }));
    }

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


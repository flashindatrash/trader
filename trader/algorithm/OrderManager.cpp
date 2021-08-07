#include "Logger.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/Database.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "algorithm/OrderManager.hpp"

static const std::string& sDbKeyOrder = "order:";
static const std::string& sDbKeyLastOrder = "stats:time_last:";
static const std::string& sDbKeyProfit = "stats:profit:";

OrderManager::OrderManager(const TradeSymbol& symbol)
{
    _orders = SOrders().getAllOrders(symbol);
    _last_time = DB().getAsLong(sDbKeyLastOrder + symbol);

    // найдем все открытые позиции
    std::vector<std::string> keys = DB().keys(sDbKeyOrder + "*");
    for (const BinanceOrderData& order : _orders) {
        std::string id = sDbKeyOrder + order.clientOrderId;
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
            if (position.side == "BUY")
                std::cout << "+";
            else
                std::cout << "-";
        }
        std::cout << std::endl;
    }
}

bool OrderManager::create(const TradeSymbol& symbol, const std::string& side, double quantity, const BinanceOrderData* transaction) {
    // проверяем, что достаточно средств
    if (not SOrders().isEnough(symbol, side, quantity))
        return false;

    BinanceOrderData result = SOrders().createOrder(symbol, side, quantity);
    // неизвестная ошибка
    if (result.isEmpty())
        return false;

    // обновляем время даже если нам вернули сстатус REJECTED
    updateLastTime(symbol);

    // не удалось создать
    if (result.isRejected())
        return false;

    // сохраним историю
    _orders.push_back(result);

    // открыть/закрыть транзакцию
    if (transaction == nullptr) {
        trace("\a%s %f %s for %f\n", side.c_str(), quantity, symbol.baseAsset().c_str(), symbol.getPrice());
        open(result);
    } else {
        trace("\a%s %f %s for %f (prev %f)\n", side.c_str(), quantity, symbol.baseAsset().c_str(), symbol.getPrice(), transaction->getPrice());
        double profit = std::abs(transaction->getPrice() - symbol.getPrice()) * transaction->quantity;
        addProfitStats(profit, symbol.quoteAsset());
        close(transaction->clientOrderId);
    }

    return true;
}

void OrderManager::updateLastTime(const TradeSymbol& symbol) {
    _last_time = STime().getCurrent();
    DB().set(sDbKeyLastOrder + symbol, _last_time);
}

void OrderManager::open(const BinanceOrderData& transaction) {
    DB().set(sDbKeyOrder + transaction.clientOrderId, true);
    _positions.push_back(transaction);
}

void OrderManager::close(const std::string& transaction_id) {
    DB().del(sDbKeyOrder + transaction_id);
    size_t size = _positions.size();
    _positions.erase(std::remove_if(_positions.begin(), _positions.end(), [transaction_id](BinanceOrderData t) { return t.clientOrderId == transaction_id; }));
    if (size == _positions.size())
        logic_error("transaction wasn't closed");
}

void OrderManager::addProfitStats(double profit, const TradeAsset& asset) {
    std::string balance_key = sDbKeyProfit + asset;
    double profit_total = profit + DB().getAsDouble(balance_key);
    DB().set(balance_key, profit_total);
    trace("%sprofit update: +%.2f (total +%.2f) %s%s\n", GREEN, profit, profit_total, asset.c_str(), RESET);
}

const std::vector<BinanceOrderData>& OrderManager::getOrders() const {
    return _orders;
}

const std::vector<BinanceOrderData>& OrderManager::getTransactions() const {
    return _positions;
}

time_t OrderManager::getLastTime() const {
    return _last_time;
}


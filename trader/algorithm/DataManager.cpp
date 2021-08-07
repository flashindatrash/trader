#include "proxy/Database.hpp"
#include "algorithm/DataManager.hpp"

const std::string& DataManager::sDbKeyOrder = "order:";
const std::string& DataManager::sDbKeyLastOrder = "stats:time_last:";
const std::string& DataManager::sDbKeyProfit = "stats:profit:";

std::vector<std::string> DataManager::getPositionIds() {
    return DB().keys(sDbKeyOrder + "*");
}

void DataManager::openPosition(const std::string& order_id) {
    DB().set(sDbKeyOrder + order_id, true);
}

void DataManager::closePosition(const std::string& order_id) {
    DB().del(sDbKeyOrder + order_id);
}

time_t DataManager::getLastOrderTime(const std::string& symbol) {
    return DB().getAsLong(sDbKeyLastOrder + symbol);
}

void DataManager::setLastOrderTime(const std::string& symbol, time_t time) {
    DB().set(sDbKeyLastOrder + symbol, time);
}

double DataManager::addProfit(const std::string& asset, double profit) {
    profit += DB().getAsDouble(sDbKeyProfit + asset);
    DB().set(sDbKeyProfit + asset, profit);
    return profit;
}

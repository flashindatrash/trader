#include "DataManager.hpp"
#include "database/Database.hpp"

using namespace scalping;

const std::string& DataManager::sDbKeyOrder = "order:";
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

double DataManager::addProfit(const std::string& asset, double profit) {
    profit += DB().getAsDouble(sDbKeyProfit + asset);
    DB().set(sDbKeyProfit + asset, profit);
    return profit;
}

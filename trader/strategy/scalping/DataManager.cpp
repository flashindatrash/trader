#include "DataManager.hpp"
#include "database/Database.hpp"

using namespace scalping;

const std::string& DataManager::sDbKeyOrder = "order:";
const std::string& DataManager::sDbKeyProfit = "stats:profit:";

std::vector<std::string> DataManager::getPositionIds() {
    return DB().keys(sDbKeyOrder + "*");
}

void DataManager::openPosition(const std::string& order_id) {
    DB().set(sDbKeyOrder + order_id, database::Value(true));
}

void DataManager::closePosition(const std::string& order_id) {
    DB().del(sDbKeyOrder + order_id);
}

double DataManager::addProfit(const std::string& asset, double profit) {
    profit += DB().get(sDbKeyProfit + asset).asDouble();
    DB().set(sDbKeyProfit + asset, database::Value(profit));
    return profit;
}

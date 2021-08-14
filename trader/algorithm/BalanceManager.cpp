#include "Logger.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "algorithm/BalanceManager.hpp"

void BalanceManager::add(const BinanceBalanceData& balance) {
    _balances[balance.asset] = balance.free;
}

void BalanceManager::print(const Symbol& symbol) {
    if (not _balances.empty()) {
        std::string message = "";
        for (auto& pair : _balances) {
            if (pair.first == symbol.baseAsset() || pair.first == symbol.quoteAsset())
                message += pair.first + "(" + std::to_string(pair.second) + ") ";
        }
        if (not message.empty())
            trace("balance update: %s\n", message.c_str());
        _balances.clear();
    }
}

#include "Logger.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "algorithm/BalanceManager.hpp"

void BalanceManager::add(const BinanceBalanceData& balance) {
    _balances.push_back(balance);
}

bool BalanceManager::check(const Symbol& symbol) {
    if (not _balances.empty()) {
        std::string message = "";
        for (const BinanceBalanceData &data : _balances) {
            if (data.asset == symbol.baseAsset() || data.asset == symbol.quoteAsset())
                message += data.asset + "(" + std::to_string(data.free) + ") ";
        }
        if (not message.empty())
            trace("balance update: %s\n", message.c_str());
        _balances.clear();
    }
    return false;
}

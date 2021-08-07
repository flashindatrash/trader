#include "Logger.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "algorithm/BalanceManager.hpp"

void BalanceManager::add(const BinanceBalanceData& balance) {
    _balances.push_back(balance);
}

bool BalanceManager::check(const TradeSymbol& symbol) {
    if (not _balances.empty()) {
        std::string message = "";
        for (const BinanceBalanceData &data : _balances) {
            if (data.asset == symbol.baseAsset() || data.asset == symbol.quoteAsset())
                message += data.asset + "(" + std::to_string(data.free) + ") ";
        }
        if (not message.empty())
            trace("%sbalance update: %s%s\n", GREEN, message.c_str(), RESET);
        _balances.clear();
    }
    return false;
}

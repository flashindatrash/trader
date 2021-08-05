#include "Logger.hpp"
#include "proxy/Database.hpp"
#include "wrapper/BinanceSymbol.hpp"
#include "data/BinanceBalanceData.hpp"
#include "data/BinanceBookData.hpp"
#include "algorithm/Algorithm.hpp"
#include "algorithm/OrderPool.hpp"
#include "algorithm/ProfitManager.hpp"
#include "algorithm/TraderManager.hpp"

Algorithm::Algorithm(const BinanceSymbol& symbol)
{
    trace("%s%s: %f %s: %f%s\n", GREEN, symbol.baseAsset().c_str(), symbol.baseAsset().getBalance(), symbol.quoteAsset().c_str(), symbol.quoteAsset().getBalance(), RESET);

    _pool = new OrderPool(symbol);
    _profit_manager = new ProfitManager(*_pool);
    _trader_manager = new TraderManager(*_pool);
}

Algorithm::~Algorithm() {
    SAFE_DELETE(_pool);
    SAFE_DELETE(_profit_manager);
    SAFE_DELETE(_trader_manager);
}

void Algorithm::onBookChanged(const BinanceBookData& data) {
    const BinanceSymbol& symbol(data.symbol);

    if (_profit_manager->check(symbol))
        return;

    if (_trader_manager->check(symbol))
        return;
}

void Algorithm::onBalanceChanged(const BinanceBalanceData &data) {
    trace("%s%s: %f%s\n", GREEN, data.asset.c_str(), data.free, RESET);
}

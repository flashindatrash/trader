#include "Logger.hpp"
#include "proxy/BinanceAlgorithm.hpp"
#include "proxy/BinanceAccount.hpp"
#include "proxy/BinancePrices.hpp"
#include "proxy/BinanceKlines.hpp"
#include "algorithm/Migrator.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/ProfitManager.hpp"
#include "algorithm/TraderManager.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "data/BinanceBalanceData.hpp"
#include "data/BinanceKlineData.hpp"

BinanceAlgorithm::~BinanceAlgorithm() {
    SAFE_DELETE(_pool);
    SAFE_DELETE(_profit_manager);
    SAFE_DELETE(_trader_manager);
}

void BinanceAlgorithm::init(const TradeSymbol& symbol) {
    trace("%s%s: %f %s: %f%s\n", GREEN, symbol.baseAsset().c_str(), symbol.baseAsset().getBalance(), symbol.quoteAsset().c_str(), symbol.quoteAsset().getBalance(), RESET);

    _pool = new OrderManager(symbol);
    if (not Migrator::migrate(*_pool))
        return;

    _profit_manager = new ProfitManager(*_pool);
    _trader_manager = new TraderManager(*_pool);

    SAccount().addListener(std::bind(&BinanceAlgorithm::onBalanceChanged, this, std::placeholders::_1));
    SPrices().addListener(std::bind(&BinanceAlgorithm::onBookChanged, this, std::placeholders::_1));
    SKlines().addListener(std::bind(&BinanceAlgorithm::onKlineChanged, this, std::placeholders::_1));
}

void BinanceAlgorithm::onBalanceChanged(const BinanceBalanceData &data) {
    trace("%s%s: %f%s\n", GREEN, data.asset.c_str(), data.free, RESET);
}

void BinanceAlgorithm::onBookChanged(const BinanceBookData& data) {
}

void BinanceAlgorithm::onKlineChanged(const BinanceKlineData &data) {
    const TradeSymbol& symbol(data.symbol);

    if (_profit_manager->check(symbol))
        return;

    if (_trader_manager->check(symbol))
        return;
}

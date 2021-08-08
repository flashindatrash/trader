#include "Logger.hpp"
#include "proxy/BinanceAlgorithm.hpp"
#include "proxy/BinanceAccount.hpp"
#include "proxy/BinanceKlines.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "data/BinanceKlineData.hpp"
#include "algorithm/Migrator.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/BalanceManager.hpp"
#include "algorithm/ProfitManager.hpp"
#include "algorithm/TraderManager.hpp"

BinanceAlgorithm::~BinanceAlgorithm() {
    SAFE_DELETE(_pool);
    SAFE_DELETE(_balance_manager);
    SAFE_DELETE(_profit_manager);
    SAFE_DELETE(_trader_manager);
}

void BinanceAlgorithm::init(const TradeSymbol& symbol) {    
    _pool = new OrderManager(symbol);
    if (not Migrator::migrate(*_pool))
        return;

    _balance_manager = new BalanceManager();
    _profit_manager = new ProfitManager(*_pool);
    _trader_manager = new TraderManager(*_pool);

    _balance_manager->add({symbol.baseAsset(), symbol.baseAsset().getBalance()});
    _balance_manager->add({symbol.quoteAsset(), symbol.quoteAsset().getBalance()});

    SAccount().addListener(std::bind(&BinanceAlgorithm::onBalanceChanged, this, std::placeholders::_1));
    SKlines().addListener(std::bind(&BinanceAlgorithm::onKlineChanged, this, std::placeholders::_1));
}

void BinanceAlgorithm::onBalanceChanged(const BinanceBalanceData &data) {
    _balance_manager->add(data);
}

void BinanceAlgorithm::onKlineChanged(const BinanceKlineData &data) {
    const TradeSymbol& symbol(data.symbol);

    if (_balance_manager->check(symbol))
        return;

    if (_profit_manager->check(symbol))
        return;

    if (_trader_manager->check(symbol))
        return;
}

#include "BinanceAlgorithm.hpp"
#include "Config.hpp"
#include "proxy/TraderTime.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/wrapper/Symbol.hpp"
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

void BinanceAlgorithm::init(const core::Config& config, const Symbol& symbol) {
    _symbol = &symbol;

    _pool = new OrderManager(symbol, config.getAsInt("TEST_MODE") == 1);
    if (not Migrator::migrate(*_pool))
        return;

    _balance_manager = new BalanceManager();
    _profit_manager = new ProfitManager(*_pool);
    _trader_manager = new TraderManager(*_pool);

    _balance_manager->add(symbol.baseAsset());
    _balance_manager->add(symbol.quoteAsset());

    _profit_manager->init(symbol);
    _trader_manager->init(symbol);

    Time().onTick.connect(std::bind(&BinanceAlgorithm::tick, this, std::placeholders::_1));
    Exchanger().balances().onChanged.connect(std::bind(&BinanceAlgorithm::onBalanceChanged, this, std::placeholders::_1));
}

void BinanceAlgorithm::onBalanceChanged(const Asset& asset) {
    _balance_manager->add(asset);
}

void BinanceAlgorithm::tick(time_t now) {
    _balance_manager->tick(*_symbol);
    _profit_manager->tick(*_symbol);
    _trader_manager->tick(*_symbol);
}

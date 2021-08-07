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
#include "data/BinanceKlineData.hpp"

BinanceAlgorithm::~BinanceAlgorithm() {
    SAFE_DELETE(_pool);
    SAFE_DELETE(_profit_manager);
    SAFE_DELETE(_trader_manager);
}

void BinanceAlgorithm::init(const TradeSymbol& symbol) {
    _balances.push_back({symbol.baseAsset(), symbol.baseAsset().getBalance()});
    _balances.push_back({symbol.quoteAsset(), symbol.quoteAsset().getBalance()});

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
    _balances.push_back(data);
}

void BinanceAlgorithm::onBookChanged(const BinanceBookData& data) {
}

void BinanceAlgorithm::onKlineChanged(const BinanceKlineData &data) {
    const TradeSymbol& symbol(data.symbol);

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


    if (_profit_manager->check(symbol))
        return;

    if (_trader_manager->check(symbol))
        return;
}

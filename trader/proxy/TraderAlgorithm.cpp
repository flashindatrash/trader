#include "TraderAlgorithm.hpp"
#include "Config.hpp"
#include "proxy/TraderTime.hpp"
#include "algorithm/Migrator.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/StatusManager.hpp"
#include "algorithm/ProfitManager.hpp"
#include "algorithm/TraderManager.hpp"

TraderAlgorithm::~TraderAlgorithm() {
    SAFE_DELETE(_pool);
    SAFE_DELETE(_status_manager);
    SAFE_DELETE(_profit_manager);
    SAFE_DELETE(_trader_manager);
}

bool TraderAlgorithm::init(const core::Config& config, const Symbol& symbol) {
    _symbol = &symbol;

    _pool = new OrderManager(symbol, config.getAsInt("TEST_MODE") == 1);
    if (not Migrator::migrate(*_pool))
        return false;

    _status_manager = new StatusManager(*_pool);
    _profit_manager = new ProfitManager(*_pool);
    _trader_manager = new TraderManager(*_pool);

    _status_manager->init(symbol);
    _profit_manager->init(symbol);
    _trader_manager->init(symbol);

    Time().onTick.connect(std::bind(&TraderAlgorithm::tick, this, std::placeholders::_1));
    return true;
}

void TraderAlgorithm::tick(time_t now) {
    _status_manager->tick(*_symbol);
    _profit_manager->tick(*_symbol);
    _trader_manager->tick(*_symbol);
}

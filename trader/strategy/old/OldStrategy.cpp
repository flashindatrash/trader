#include "OldStrategy.hpp"
#include <global.hpp>
#include "OrderManager.hpp"
#include "StatusManager.hpp"
#include "ProfitManager.hpp"
#include "TraderManager.hpp"
#include "Config.hpp"
#include "Time.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"

using namespace scalping;

static const std::string& CONFIG_BASE_KEY = "BASE_ASSET";
static const std::string& CONFIG_QUOTE_KEY = "QUOTE_ASSET";

OldStrategy::~OldStrategy() {
    SAFE_DELETE(_pool);
    SAFE_DELETE(_status_manager);
    SAFE_DELETE(_profit_manager);
    SAFE_DELETE(_trader_manager);
}

bool OldStrategy::init(const core::Config& config) {
    if (not config.has(CONFIG_BASE_KEY) || not config.has(CONFIG_QUOTE_KEY))
        return false;

    _symbol = Symbol(config.asString(CONFIG_BASE_KEY), config.asString(CONFIG_QUOTE_KEY));
    if (Exchanger().pair(_symbol.id()) == nullptr)
        return false;

    Exchanger().chart(_symbol.id())->setInterval(ChartInterval::m15);
    Exchanger().loadOrders(_symbol.id());
    Exchanger().listenCharts(_symbol.id());

    _pool = new OrderManager(_symbol);
    _status_manager = new StatusManager(*_pool);
    _profit_manager = new ProfitManager(*_pool);
    _trader_manager = new TraderManager(*_pool);

    if (not _status_manager->init(_symbol)) return false;
    if (not _profit_manager->init(_symbol)) return false;
    if (not _trader_manager->init(_symbol)) return false;

    Time().onTick.connect(std::bind(&OldStrategy::tick, this, std::placeholders::_1));
    return true;
}

void OldStrategy::tick(time_t now) {
    _status_manager->tick(_symbol);
    _profit_manager->tick(_symbol);
    _trader_manager->tick(_symbol);
}

bool OldStrategy::isRunning() const {
    return true;
}

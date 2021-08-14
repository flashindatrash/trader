#include "ExchangerProxy.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "exchanger/base/ExchangerController.hpp"

#include "proxy/BinanceExchangeInfo.hpp"

ExchangerProxy::~ExchangerProxy() {
    SAFE_DELETE(_controller);
}

void ExchangerProxy::init(const core::Config& config) {
    _controller = ExchangerController::create();
    _controller->init(config);

    SExchangeInfo().init();

    _controller->getSymbolInfo(_infos);
    _controller->getAllPrices(_prices);
    _controller->getBalances(_balances);

    _controller->connectBalances(_balances);

    STime().addListener(std::bind(&ExchangerController::tick, _controller, std::placeholders::_1));
}

void ExchangerProxy::connect(const Symbol& symbol) {
    _controller->connectDailyChange(*_daily_changes.get(symbol));
}

void ExchangerProxy::run() {
    _controller->run();
}

#include "ExchangerProxy.hpp"
#include "Config.hpp"
#include "proxy/TraderTime.hpp"
#include "exchanger/base/ExchangerController.hpp"

#include "proxy/BinanceExchangeInfo.hpp"

ExchangerProxy::~ExchangerProxy() {
    SAFE_DELETE(_controller);
}

void ExchangerProxy::init(const core::Config& config) {
    _controller = ExchangerController::create();
    _controller->init(config);

    SExchangeInfo().init();

    _controller->getSymbolInfo(infos());

    _controller->connectPrices(prices());
    _controller->connectBalances(balances());

    Time().onTick.connect(std::bind(&ExchangerController::tick, _controller, std::placeholders::_1));
}

void ExchangerProxy::connect(const Symbol& symbol) {
    _controller->connectDailyChange(*daily_change(symbol));
    _controller->connectChart(*chart(symbol), ChartInterval::m5);
}

void ExchangerProxy::run() {
    _controller->run();
}

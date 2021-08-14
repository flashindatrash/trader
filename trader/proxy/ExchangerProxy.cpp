#include "ExchangerProxy.hpp"
#include "Config.hpp"
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

    _controller->getSymbolInfo(infos());
    _controller->getAllPrices(prices());
    _controller->getBalances(balances());

    _controller->connectBalances(balances());

    STime().addListener(std::bind(&ExchangerController::tick, _controller, std::placeholders::_1));
}

void ExchangerProxy::run() {
    _controller->run();
}

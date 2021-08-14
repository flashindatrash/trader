#include "ExchangerProxy.hpp"
#include "Config.hpp"
#include "proxy/BinanceTime.hpp"
#include "exchanger/ExchangerController.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"

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

    _controller->connectBalances(_balances, onBalanceChanged);

    STime().addListener(std::bind(&ExchangerController::tick, _controller, std::placeholders::_1));
}

void ExchangerProxy::run() {
    _controller->run();
}

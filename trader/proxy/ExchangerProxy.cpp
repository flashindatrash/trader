#include "ExchangerProxy.hpp"
#include <global.hpp>
#include "Config.hpp"
#include "exchanger/ExchangerController.hpp"
#include "exchanger/wrapper/SymbolSet.hpp"
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
}

void ExchangerProxy::run() {
    _controller->run();
}

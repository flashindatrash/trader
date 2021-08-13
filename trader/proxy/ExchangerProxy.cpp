#include "ExchangerProxy.hpp"
#include <global.hpp>
#include "Config.hpp"
#include "exchanger/ExchangerController.hpp"
#include "exchanger/SymbolSet.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"

#include "proxy/BinanceExchangeInfo.hpp"

ExchangerProxy::~ExchangerProxy() {
    SAFE_DELETE(_controller);
}

void ExchangerProxy::init(const core::Config& config) {
    _controller = ExchangerController::create();
    _controller->init(config);

    SExchangeInfo().init();

    std::vector<std::pair<Symbol, Price>> prices = _controller->getAllPrices();
    for (const auto& pair : prices)
        price_mutable(pair.first)->add(pair.second);
}

void ExchangerProxy::run() {
    _controller->run();
}

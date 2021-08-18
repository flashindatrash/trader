#include "Exchanger.hpp"
#include "Config.hpp"
#include "proxy/Time.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/base/ExchangerController.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"

ExchangerProxy::~ExchangerProxy() {
    SAFE_DELETE(_controller);
}

bool ExchangerProxy::init(const core::Config& config) {
    _controller = ExchangerController::create();
    if (not _controller->init(config))
        return false;

    if (not _controller->getSymbolInfo(_pairs))
        return false;

    _controller->connectPrices(_prices);
    _controller->connectBalances(_balances);
    _controller->connectStats(_stats);
    _controller->connectChart(_charts);

    Time().onTick.connect(std::bind(&ExchangerController::tick, _controller, std::placeholders::_1));
    return true;
}

const OrderWrapper* ExchangerProxy::createOrder(const OrderRequest& request) {
    return _controller->createOrder(request);
}

void ExchangerProxy::run() {
    _controller->run();
}

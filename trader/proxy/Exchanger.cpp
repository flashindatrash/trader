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

    if (not _controller->loadPairs(_pairs))
        return false;

    _controller->connectPrices(_prices);
    _controller->connectBalances(_balances);
    _controller->connectCharts(_charts);

    Time().onTick.connect(std::bind(&ExchangerProxy::tick, this, std::placeholders::_1));
    return true;
}

void ExchangerProxy::run() {
    _controller->run();
}

void ExchangerProxy::tick(time_t now) {
    _controller->tick(now);
}

bool ExchangerProxy::loadOrders(const std::string& key) {
    return _controller->loadOrders(*_books.get(key));
}

void ExchangerProxy::listenCharts(const std::string& key, ChartInterval interval) {
    _controller->listenCharts(*_charts.get(key), interval);
}

const OrderWrapper* ExchangerProxy::createOrder(const OrderRequest& request) {
    return _controller->createOrder(*_books.get(request.symbol), request);
}



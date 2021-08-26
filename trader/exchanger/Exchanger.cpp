#include "Exchanger.hpp"
#include "Config.hpp"
#include "Time.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/base/ExchangerController.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"

ExchangerProxy::~ExchangerProxy() {
    stop();
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

void ExchangerProxy::stop() {
    if (_controller != nullptr) {
        delete _controller;
        _controller = nullptr;
    }
}

void ExchangerProxy::tick(time_t now) {
    _controller->tick(now);
}

bool ExchangerProxy::loadOrders(const std::string& key) {
    return _controller->loadOrders(*_books.get(key));
}

bool ExchangerProxy::loadCharts(const std::string& key, ChartInterval interval) {
    return _controller->loadCharts(*_charts.get(key), interval);
}

void ExchangerProxy::listenCharts(const std::string& key, ChartInterval interval) {
    _controller->listenCharts(*_charts.get(key), interval);
}

const OrderWrapper* ExchangerProxy::createOrder(const OrderRequest& request) {
    return _controller->createOrder(*_books.get(request.symbol), request);
}

double ExchangerProxy::minQuantity(const std::string& key) const {
    return _controller->minQuantity(key);
}



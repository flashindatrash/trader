#include "Exchanger.hpp"
#include "core/Config.hpp"
#include "core/Time.hpp"
#include "exchanger/abstract/ExchangerController.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/StakingWrapper.hpp"

ExchangerProxy::~ExchangerProxy() {
    stop();
}

bool ExchangerProxy::init(const core::Config& config) {
    _controller = ExchangerController::create();
    if (not _controller->init(config))
        return false;

    if (not loadPairs())
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

bool ExchangerProxy::loadPairs() {
    return _controller->loadPairs(_pairs);
}

bool ExchangerProxy::loadPrices() {
    return _controller->loadPrices(_prices);
}

bool ExchangerProxy::loadStakings() {
    return _controller->loadStakings(_stakings);
}

bool ExchangerProxy::loadPrice(const std::string& key) {
    return _controller->loadPrice(*_prices.get(key));
}

bool ExchangerProxy::loadOrders(const std::string& key) {
    return _controller->loadOrders(*_books.get(key));
}

bool ExchangerProxy::loadStats(const std::string& key) {
    return _controller->loadStats(*_stats.get(key));
}

bool ExchangerProxy::loadCharts(const std::string& key, ChartRequest& request) {
    return _controller->loadCharts(*_charts.get(key), request);
}

void ExchangerProxy::listenCharts(const std::string& key, ChartInterval interval) {
    _controller->listenCharts(*_charts.get(key), interval);
}

void ExchangerProxy::listenTickers(const std::string& key) {
    _controller->listenTicker(*_prices.get(key));
}

void ExchangerProxy::unlistenTickers(const std::string& key) {
    _controller->unlistenTicker(*_prices.get(key));
}

bool ExchangerProxy::updateStaking(const std::string& key) {
    return _controller->updateStaking(*_stakings.get(key));
}

const OrderWrapper* ExchangerProxy::createOrder(OrderRequest& request) {
    return _controller->createOrder(*_books.get(request.symbol), request);
}

bool ExchangerProxy::stake(StakingRequest& request) {
    return _controller->stake(*_stakings.get(request.projectId), request);
}

double ExchangerProxy::roundQuantity(double quantity, const std::string& key, double(*fn)(double)) const {
    return _controller->roundQuantity(quantity, key, fn);
}

double ExchangerProxy::fee() const {
    return _controller->fee();
}


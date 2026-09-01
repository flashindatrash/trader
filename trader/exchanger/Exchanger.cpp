#include "Exchanger.hpp"
#include "core/Config.hpp"
#include "core/Time.hpp"
#include "exchanger/abstract/ExchangerController.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"

ExchangerProxy::ExchangerProxy() = default;

ExchangerProxy::~ExchangerProxy() {
    stop();
}

bool ExchangerProxy::init(const core::Config& config) {
    _controller = ExchangerController::create(config);
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
    _controller.reset();
}

void ExchangerProxy::tick(time_t now) {
    _controller->tick(now);
}

bool ExchangerProxy::loadPairs() {
    return _controller->loadPairs(_pairs);
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

const OrderWrapper* ExchangerProxy::createOrder(OrderRequest& request) {
    return _controller->createOrder(*_books.get(request.symbol), request);
}

Decimal ExchangerProxy::roundQuantity(Decimal quantity, const std::string& key) const {
    return _controller->roundQuantity(quantity, key);
}

double ExchangerProxy::fee() const {
    return _controller->fee();
}

#include "ExchangerProxy.hpp"
#include "Config.hpp"
#include "proxy/TraderTime.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/base/ExchangerController.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"

ExchangerProxy::~ExchangerProxy() {
    SAFE_DELETE(_controller);
}

bool ExchangerProxy::init(const core::Config& config, const Symbol& symbol) {
    _controller = ExchangerController::create();
    if (not _controller->init(config))
        return false;

    if (not _controller->getSymbolInfo(_pairs))
        return false;

    _controller->connectPrices(_prices);
    _controller->connectBalances(_balances);

    _stat_connector = _controller->connectStats(*_stats.get(symbol));
    _chart_connector = _controller->connectChart(*_charts.get(symbol), ChartInterval::m15);
    _book_connector = _controller->connectOrders(*_books.get(symbol));

    if (!_stat_connector || !_chart_connector || !_book_connector)
        return false;

    Time().onTick.connect(std::bind(&ExchangerController::tick, _controller, std::placeholders::_1));
    return true;
}

const OrderWrapper* ExchangerProxy::createOrder(const OrderRequest& request) {
    return _controller->createOrder(request);
}

void ExchangerProxy::run() {
    _controller->run();
}

#include "ExchangerProxy.hpp"
#include "Config.hpp"
#include "Logger.hpp"
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

KlineWrapper* ExchangerProxy::getDailyChange(const Symbol& symbol) {
    KlineWrapper* wrapper = daily_change(symbol);
    if (wrapper->timeClose() > STime().getCurrent() - BinanceTime::sMinute * 30)
        return wrapper;

    if (not _controller->getDailyChange(*wrapper, symbol))
        Logger::error("ExchangerProxy::getDailyChange");

    return wrapper;
}

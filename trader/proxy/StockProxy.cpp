#include <global.hpp>
#include "Config.hpp"
#include "proxy/StockProxy.hpp"
#include "controller/StockController.hpp"

StockProxy::~StockProxy() {
    SAFE_DELETE(_controller);
}

void StockProxy::init(const core::Config& config) {
    _controller = StockController::create();
    _controller->init(config);
}

void StockProxy::run() {
    _controller->run();
}

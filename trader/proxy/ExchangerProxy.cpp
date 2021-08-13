#include "ExchangerProxy.hpp"
#include <global.hpp>
#include "Config.hpp"
#include "exchanger/ExchangerController.hpp"

ExchangerProxy::~ExchangerProxy() {
    SAFE_DELETE(_controller);
}

void ExchangerProxy::init(const core::Config& config) {
    _controller = ExchangerController::create();
    _controller->init(config);
}

void ExchangerProxy::run() {
    _controller->run();
}

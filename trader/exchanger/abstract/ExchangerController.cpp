#include "ExchangerController.hpp"
#include "exchanger/gate/GateController.hpp"

std::unique_ptr<ExchangerController> ExchangerController::create() {
    return std::make_unique<GateController>();
}

std::unique_ptr<ExchangerController> ExchangerController::create(const core::Config&) {
    return create();
}

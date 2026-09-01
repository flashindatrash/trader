#include "ExchangerController.hpp"
#include "exchanger/binance/BinanceController.hpp"
#include "exchanger/gate/GateController.hpp"
#include "core/Config.hpp"
#include "util/StringUtil.hpp"

std::unique_ptr<ExchangerController> ExchangerController::create() {
    return std::make_unique<BinanceController>();
}

std::unique_ptr<ExchangerController> ExchangerController::create(const core::Config& config) {
    const std::string exchange = util::lowercase(config.asString("EXCHANGE").c_str());
    if (exchange == "gate" || exchange == "gate.io" || exchange == "gateio")
        return std::make_unique<GateController>();
    return create();
}

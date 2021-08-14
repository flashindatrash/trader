#include "ExchangerController.hpp"
#include "exchanger/binance/BinanceController.hpp"

ExchangerController* ExchangerController::create() {
    return new BinanceController();
}

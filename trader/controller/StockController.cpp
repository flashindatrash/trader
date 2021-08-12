#include "controller/StockController.hpp"
#include "controller/binance/BinanceController.hpp"

StockController* StockController::create() {
    return new BinanceController();
}

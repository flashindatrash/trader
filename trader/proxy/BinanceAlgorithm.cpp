#include "proxy/BinanceAlgorithm.hpp"
#include "proxy/BinancePrices.hpp"
#include "wrapper/algorithm/DebugAlgorithm.hpp"

BinanceAlgorithm::~BinanceAlgorithm() {
    if (_algorithm != nullptr) {
        delete _algorithm;
        _algorithm = nullptr;
    }
}

void BinanceAlgorithm::init(const BinanceSymbol& symbol) {
    _algorithm = new DebugAlgorithm(symbol);
    SBinancePrices().addListener(std::bind(&BinanceAlgorithm::handle, this, std::placeholders::_1));
}

void BinanceAlgorithm::handle(const BinanceBookData& data) {
    _algorithm->handle(data);
}

#include "proxy/BinanceAlgorithm.hpp"
#include "proxy/BinanceAccount.hpp"
#include "proxy/BinancePrices.hpp"
#include "algorithm/Algorithm.hpp"

BinanceAlgorithm::~BinanceAlgorithm() {
    SAFE_DELETE(_algorithm);
}

void BinanceAlgorithm::init(const BinanceSymbol& symbol) {
    _algorithm = new Algorithm(symbol);
    SAccount().addListener(std::bind(&Algorithm::onBalanceChanged, _algorithm, std::placeholders::_1));
    SPrices().addListener(std::bind(&Algorithm::onBookChanged, _algorithm, std::placeholders::_1));
}

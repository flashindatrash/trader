#include "Algorithm.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "BookManager.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

NS_USE

Algorithm* Algorithm::create(const Settings& settings) {
    Algorithm* algorithm = new Algorithm();
    return algorithm;
}

bool Algorithm::init() {
    _book = BookManager::create();
    if (not _book->init()) {
        return false;
    }

    return true;;
}

void Algorithm::execute(const Context& context) {
    Logger::info("candle price: %f", context.candlestick->priceClose());
}

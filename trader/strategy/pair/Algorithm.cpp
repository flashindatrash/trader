#include "Algorithm.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Positions.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

NS_USE

Algorithm* Algorithm::create(const Settings& settings) {
    Algorithm* algorithm = new Algorithm(settings);
    return algorithm;
}

Algorithm::Algorithm(const Settings& settings)
    : _settings(settings)
{
}

bool Algorithm::init() {
    _positions = Positions::create(_settings.symbol);
    return true;
}

void Algorithm::execute(const Context& context) {
    Change change = util::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    if (std::abs(change) > 0.001)
        Logger::info("candle price change: %f", change);

}

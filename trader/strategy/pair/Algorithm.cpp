#include "Algorithm.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

NS_USE

Algorithm* Algorithm::create(const Settings& settings) {
    Algorithm* algorithm = new Algorithm();
    return algorithm;
}

#include "database/Array.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
bool Algorithm::init() {
    static const std::string test = "test";

    db::Objects arr("b1");

    db::Object obj("o3");
    obj.set(test, "wtf");
    if (obj.flush()) {
        if (not arr.push(obj)) Logger::info("ke1");
        if (not arr.push(obj)) Logger::info("ke2");
    }

    for (auto& it : arr) {
        Logger::info("key %s", it.get(test).asCString());
    }

    return true;
}

void Algorithm::execute(const Context& context) {
    Change change = util::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    if (std::abs(change) > 0.001)
        Logger::info("candle price change: %f", change);

}

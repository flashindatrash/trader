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

    db::ArrayObject arr("b1");

    db::Object obj("o5");
    obj.set(test, "wtf");

    arr.push(obj);
    arr.remove("o4");

    for (auto& it : arr) {
        Logger::info("key %s", it.get(test).asCString());
    }

    db::Array a("a1");

    db::Value v = db::Value(2);
    a.push(v);

    for (auto& it : a) {
        Logger::info("a %s", it.asCString());
    }

    return true;
}

void Algorithm::execute(const Context& context) {
    Change change = util::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    if (std::abs(change) > 0.001)
        Logger::info("candle price change: %f", change);

}

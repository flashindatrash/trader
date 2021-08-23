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

    /*db::ArrayObject arr("b1");

    db::Object obj("test_types8");
    obj.set("bool", true);
    obj.set("int", 12);
    obj.set("double", 1.00023);
    arr.push(obj);*/

    db::Array a("a1");
    a.push(2);
    a.push(false);
    a.push(232.44);

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

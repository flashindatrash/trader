#include "Algorithm.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Positions.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

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
    _positions = Positions::create(_settings.symbol, not _settings.test);

    Position pos1("test1");
    pos1.setPrice(0.2);
    pos1.setQuantity(20);
    pos1.setSide(OrderSide::Sell);
    _positions->push(pos1);

    Position pos2("test2");
    pos2.setPrice(0.3);
    pos2.setQuantity(1.1);
    pos2.setSide(OrderSide::Buy);
    _positions->push(pos2);

    return true;
}

void Algorithm::execute(const Context& context) {
    tryClosePosition(context) || tryOpenPosition(context);
}

bool Algorithm::tryClosePosition(const Context& context) {
    return false;
}

bool Algorithm::tryOpenPosition(const Context& context) {
    OrderRequest request;
    request.side = OrderSide::Buy;
    request.quantity = 0.2;

    auto equal = _positions->find_if([context, request](const Position& position) {
        return position.side() == request.side;
    });

    if (equal != _positions->cend())
        return false;

    return false;
}

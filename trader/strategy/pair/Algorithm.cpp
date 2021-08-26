#include "Algorithm.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Positions.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/Exchanger.hpp"

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

    Position pos3("test3");
    pos3.setPrice(0.25);
    pos3.setQuantity(1.1);
    pos3.setSide(OrderSide::Buy);
    _positions->push(pos3);

    size_t c = _positions->count_if(Predicates::combine(Predicates::sell, Predicates::less(0.3)));
    auto it = _positions->compare_if(Predicates::buy, Compares::min);
    return true;
}

void Algorithm::execute(const Context& context) {
    tryClosePosition(context) || tryOpenPosition(context);
}

bool Algorithm::tryClosePosition(const Context& context) {
    return false;
}

bool Algorithm::tryOpenPosition(const Context& context) {
    Change change = util::change(context.candlestick->priceOpen(), context.candlestick->priceClose());

    OrderRequest request;
    request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;

    if (request.side == OrderSide::Invalid)
        return false;

    Positions::Predicate predicate = Predicates::side(request.side);
    Positions::Compare compare = request.side == OrderSide::Sell ? Compares::max : Compares::min;
    Positions::const_iterator iter = _positions->compare_if(predicate, compare);


    if (iter == _positions->cend()) {
        request.quantity = Exchanger().minQuantity(context.candlestick->id());
    } else if (request.side == OrderSide::Sell) {
    }

    if (not request.isEnough())
        return false;

    return false;
}

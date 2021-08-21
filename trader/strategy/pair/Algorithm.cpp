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

#include "Position.hpp"
#include "Book.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
bool Algorithm::init() {
    Book* book = Book::byId("b2");

    for (auto p : book->positions())
        Logger::info("ss %s %s %f", p->id().c_str(), p->symbol().c_str(), p->price());

    book->remove("o2");

    Position* pos = Position::create("o5");
    pos->setPrice(0.3);
    pos->setSide(OrderSide::Sell);
    pos->setSymbol(Symbol("BTCUSDT"));
    if (not book->add(pos))
        delete pos;

    return true;
}

void Algorithm::execute(const Context& context) {
    Change change = util::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    if (std::abs(change) > 0.001)
        Logger::info("candle price change: %f", change);

}

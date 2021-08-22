#include "TraderManager.hpp"
#include "Logger.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/CandlestickPattern.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DecisionMaker.hpp"

static Change sMinRate = 0.005;

// скипать похожие позиции, у которых цена отличается на этот процент
static Change sEqualPosition = 0.01;

TraderManager::TraderManager(OrderManager& orders)
    : BaseManager(orders)
{
}

bool TraderManager::init(const Symbol& symbol) {
    if (not BaseManager::init(symbol))
        return false;

    DecisionMaker test(symbol, _orders.getPositions());
    double fs = test.factor(OrderSide::Sell, DecisionMaker::ForTrader);
    double fb = test.factor(OrderSide::Buy, DecisionMaker::ForTrader);
    Logger::info("factors buy(%f) sell(%f)", fb, fs);

    ChartWrapper::onCandleClosed.connect(std::bind(&TraderManager::onCloseCandle, this, std::placeholders::_1));
    return true;
}

void TraderManager::onCloseCandle(const CandlestickWrapper& wrapper) {
    const CandlestickWrapper* candlestick = Exchanger().chart()->last();
    if (candlestick == nullptr)
        return;

    OrderRequest request;
    request.side = OrderSide::Invalid;

    Change change = util::change(candlestick->priceOpen(), candlestick->priceClose());
    if (std::abs(change) >= sMinRate)
        request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;

    if (request.side == OrderSide::Invalid)
        return;

    // исключаем повторения похожих позиций
    for (const OrderWrapper* position : _orders.getPositions()) {
        if (position->side() == request.side)
            continue;

        Change change = util::change(position->price(), candlestick->priceClose());
        if (std::abs(change) < sEqualPosition)
            return;
    }

    // проверим можем ли выполонить сделку, сохранив множитель
    Symbol symbol = Exchanger().chart()->id();
    DecisionMaker decision(symbol, _orders.getPositions());
    double factor = decision.factor(request.side, DecisionMaker::ForTrader);
    if (factor < 0.5)
        return;

    if (not _orders.create(request, nullptr))
        Logger::info("failed create order");
}

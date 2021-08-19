#include "TraderManager.hpp"
#include "OrderManager.hpp"
#include "DecisionMaker.hpp"
#include "Logger.hpp"
#include "proxy/Exchanger.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/CandlestickPattern.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

static Change sMinRate = 0.005;

// скипать похожие позиции, у которых цена отличается на этот процент
static Change sEqualPosition = 0.004;

TraderManager::TraderManager(OrderManager& orders)
    : BaseManager(orders)
{
}

bool TraderManager::init(const Symbol& symbol) {
    if (not BaseManager::init(symbol))
        return false;

    ChartWrapper::onCandleClosed.connect(std::bind(&TraderManager::onCloseCandle, this, std::placeholders::_1));
    return true;
}

void TraderManager::onCloseCandle(const CandlestickWrapper& wrapper) {
    OrderRequest request;
    request.symbol = wrapper.id();
    request.side = OrderSide::Invalid;

    Change change = util::change(wrapper.priceOpen(), wrapper.priceClose());
    if (std::abs(change) >= sMinRate)
        request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;

    if (request.side == OrderSide::Invalid)
        return;

    // исключаем повторения похожих позиций
    for (const OrderWrapper* position : _orders.getPositions()) {
        if (position->side() == request.side)
            continue;

        Change change = util::change(position->price(), wrapper.priceClose());
        if (std::abs(change) < sEqualPosition)
            return;
    }

    // проверим можем ли выполонить сделку, сохранив множитель
    DecisionMaker decision(_orders.getPositions());
    double factor = decision.factor(request, DecisionMaker::ForTrader);
    if (factor < 0.5)
        return;

    if (not _orders.create(request, nullptr))
        Logger::info("failed create order");
}

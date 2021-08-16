#include "TraderManager.hpp"
#include "Logger.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/CandlestickPattern.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DecisionMaker.hpp"
#include "util/PriceUtil.hpp"

static double sMinRate = 0.004;

// мин объем валюты, с которым бот открывает новые заказы
// данное число умножается на минимальный разрешенный лот
static double sMinQuantity = 1.3;

TraderManager::TraderManager(OrderManager& orders)
    : BaseManager(orders)
{
}

bool TraderManager::init(const Symbol& symbol) {
    if (not BaseManager::init(symbol))
        return false;

    // устанавливаем стоимость покупки
    double min = util::get_min_quantity(symbol);
    _min_quantity = min * sMinQuantity;
    _min_quantity = util::ceil_quantity(symbol, _min_quantity/* * std::max(factor, 1.0)*/);
    Logger::info("lot quantity: %f", _min_quantity);

    ChartWrapper::onCandleClosed.connect(std::bind(&TraderManager::onCloseCandle, this, std::placeholders::_1));
    return true;
}

void TraderManager::onCloseCandle(const CandlestickWrapper& wrapper) {
    const std::vector<CandlestickWrapper*>& candlesticks = Exchanger().chart()->get();
    if (candlesticks.size() < 2)
        return;

    const CandlestickWrapper* current = *(candlesticks.end() - 1);
    const CandlestickWrapper* previous = *(candlesticks.end() - 2);

    CandlestickPattern::Pattern pattern = CandlestickPattern::find(*current, *previous);

    OrderRequest request;
    request.quantity = _min_quantity;

    Change change = PriceRange(current->priceOpen(), current->priceClose()).change();
    if (std::abs(change) >= sMinRate)
        request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;

    switch (pattern) {
    case CandlestickPattern::Hammer:            request.side = OrderSide::Buy; break;
    case CandlestickPattern::InvertedHammer:    request.side = OrderSide::Buy; break;
    case CandlestickPattern::HangingMan:        request.side = OrderSide::Sell; break;
    case CandlestickPattern::ShootingStar:      request.side = OrderSide::Sell; break;
    //case CandlestickWrapper::BullishEngulfing:  request.side = OrderSide::Sell; break;
    //case CandlestickWrapper::BearishEngulfing:  request.side = OrderSide::Buy; break;
    //case CandlestickPattern::BullishHarami:     request.side = OrderSide::Buy; break;
    //case CandlestickPattern::BearishHarami:     request.side = OrderSide::Sell; break;
    //case CandlestickPattern::BullishKicker:     request.side = OrderSide::Buy; break;
    //case CandlestickPattern::BearishKicker:     request.side = OrderSide::Sell; break;
    default: break;
    }

    if (request.side == OrderSide::Invalid)
        return;

    // проверим можем ли выполонить сделку, сохранив множитель
    Symbol symbol = Exchanger().chart()->getIdentifier();
    DecisionMaker decision(symbol);
    double factor = decision.factor(request.side, DecisionMaker::ForTrader);
    if (factor < 0.7)
        return;

    if (not _orders.create(request, nullptr))
        Logger::info("failed create order");
}

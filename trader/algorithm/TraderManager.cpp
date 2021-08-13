#include "TraderManager.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/CandlestickContainer.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/binance/response/BinanceKlineData.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DecisionMaker.hpp"
#include "util/PriceUtil.hpp"

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
    trace("trader quantity: %f\n", _min_quantity);
    return true;
}

void TraderManager::onCloseCandle(const BinanceKlineData& data) {
    const std::vector<BinanceKlineData>& candlesticks = _candlesticks->klines();
    if (candlesticks.size() < 2)
        return;

    const BinanceKlineData& current_data = *(candlesticks.end() - 1);
    const BinanceKlineData& previous_data = *(candlesticks.end() - 2);

    CandlestickWrapper current(current_data.priceOpen, current_data.priceHigh, current_data.priceLow, current_data.priceClose);
    CandlestickWrapper previous(previous_data.priceOpen, previous_data.priceHigh, previous_data.priceLow, previous_data.priceClose);

    CandlestickWrapper::Pattern pattern = current.getPattern(previous);
    trace("candle closed (pattern %d)\n", pattern);

    BinanceSideEnum side;
    switch (pattern) {
    case CandlestickWrapper::Hammer:            side = BinanceSideEnum::Buy; break;
    case CandlestickWrapper::InvertedHammer:    side = BinanceSideEnum::Buy; break;
    case CandlestickWrapper::HangingMan:        side = BinanceSideEnum::Sell; break;
    case CandlestickWrapper::ShootingStar:      side = BinanceSideEnum::Sell; break;
    case CandlestickWrapper::BullishEngulfing:  side = BinanceSideEnum::Buy; break;
    case CandlestickWrapper::BearishEngulfing:  side = BinanceSideEnum::Sell; break;
    case CandlestickWrapper::BullishHarami:     side = BinanceSideEnum::Buy; break;
    case CandlestickWrapper::BearishHarami:     side = BinanceSideEnum::Sell; break;
    case CandlestickWrapper::BullishKicker:     side = BinanceSideEnum::Buy; break;
    case CandlestickWrapper::BearishKicker:     side = BinanceSideEnum::Sell; break;
    default: return;
    }

    // проверим можем ли выполонить сделку, сохранив множитель
    Symbol symbol(data.symbol);
    DecisionMaker decision(symbol);
    double factor = decision.factor(side, DecisionMaker::ForTrader);
    trace("trader factor: %f\n", factor);
    if (factor < 0.5)
        return;

    // цена уможается до х2 зависит от фактора DecisionMaker'а
    double quantity = util::ceil_quantity(symbol, _min_quantity/* * std::max(factor, 1.0)*/);
    _orders.create(symbol, side, quantity, nullptr);
}

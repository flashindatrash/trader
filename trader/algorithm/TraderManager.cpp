#include "TraderManager.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/CandlestickPattern.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DecisionMaker.hpp"
#include "util/PriceUtil.hpp"

static double sMinRate = 0.003;

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
    return true;
}

void TraderManager::onCloseCandle(const CandlestickWrapper& wrapper) {
    const std::vector<CandlestickWrapper*>& candlesticks = _candlesticks->klines();
    if (candlesticks.size() < 2)
        return;

    const CandlestickWrapper* current = *(candlesticks.end() - 1);
    const CandlestickWrapper* previous = *(candlesticks.end() - 2);

    CandlestickPattern::Pattern pattern = CandlestickPattern::find(*current, *previous);
    Logger::info("candle closed (pattern %d)", pattern);

    SideEnum side;

    PriceRange range(current->priceOpen(), current->priceClose());
    if (std::abs(range.change()) >= sMinRate)
        side = range.change();

    switch (pattern) {
    case CandlestickPattern::Hammer:            side = SideEnum::Buy; break;
    case CandlestickPattern::InvertedHammer:    side = SideEnum::Buy; break;
    case CandlestickPattern::HangingMan:        side = SideEnum::Sell; break;
    case CandlestickPattern::ShootingStar:      side = SideEnum::Sell; break;
    //case CandlestickWrapper::BullishEngulfing:  side = SideEnum::Sell; break;
    //case CandlestickWrapper::BearishEngulfing:  side = SideEnum::Buy; break;
    case CandlestickPattern::BullishHarami:     side = SideEnum::Buy; break;
    case CandlestickPattern::BearishHarami:     side = SideEnum::Sell; break;
    case CandlestickPattern::BullishKicker:     side = SideEnum::Buy; break;
    case CandlestickPattern::BearishKicker:     side = SideEnum::Sell; break;
    default: break;
    }

    if (side == SideEnum::Invalid)
        return;

    // проверим можем ли выполонить сделку, сохранив множитель
    Symbol symbol(_candlesticks->getIdentifier());
    DecisionMaker decision(symbol);
    double factor = decision.factor(side, DecisionMaker::ForTrader);
    Logger::info("trader %s factor: %f", side.c_str(), factor);
    if (factor < 0.3)
        return;

    if (not _orders.create(symbol, side, _min_quantity, nullptr))
        Logger::info("failed create order");
}

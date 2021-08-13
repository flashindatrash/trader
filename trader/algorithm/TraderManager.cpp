#include "algorithm/TraderManager.hpp"
#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinanceKlines.hpp"
#include "exchanger/wrapper/CandlestickContainer.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/binance/response/BinanceSymbolData.hpp"
#include "exchanger/binance/response/BinanceKlineData.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/PriceAnalyzer.hpp"
#include "algorithm/DecisionMaker.hpp"
#include "util/PriceUtil.hpp"

// мин % соотношение, может требовать х2 взависимости от факторов
static float sRate = 0.007f;

// процентное соотношение цены для избегания открытия повторных схожих позиций
static float sEqualRate = 0.001f;

// мин объем валюты, с которым бот открывает новые заказы
// данное число умножается на минимальный разрешенный лот
static double sMinQuantity = 1.3;

TraderManager::TraderManager(OrderManager& orders)
    : BaseManager(orders, BinanceTime::sSecond * 30)
{
}

bool TraderManager::check(const Symbol& symbol) {
    if (not BaseManager::check(symbol))
        return false;

    // устанавливаем стоимость покупки
    if (_min_quantity == 0.0) {
        double min = util::get_min_quantity(symbol);
        _min_quantity = min * sMinQuantity;
        trace("trader quantity: %f\n", _min_quantity);
    }

    // устанавливаем враппер свеч
    if (_candlesticks == nullptr) {
        _candlesticks = SKlines().get(symbol);
        _candlesticks->addListener(std::bind(&TraderManager::onCloseCandle, this, std::placeholders::_1));
    }

    if (_candlesticks == nullptr)
        return false;

    PriceAnalyzer analyzer(*_candlesticks);
    Change change = analyzer.getStablePriceChange(_orders.getLastTime());

    // проверим можем ли выполонить сделку, сохранив множитель
    DecisionMaker decision(symbol); double factor;
    if (not decision.make(change / sRate, DecisionMaker::ForTrader, factor))
        return false;

    // не дублируем схожие позиции
    if (hasEqualPosition(change, symbol.getPrice()))
        return false;

    // цена уможается до х2 зависит от фактора DecisionMaker'а
    double quantity = util::ceil_quantity(symbol, _min_quantity * std::max(factor, 1.0));
    return _orders.create(symbol, change, quantity, nullptr);
}

bool TraderManager::hasEqualPosition(const BinanceSideEnum& side, double price) const {
    for (const BinanceOrderData& order : _orders.getPositions()) {
        // интересуют ордеры с одним типом
        if (order.side != side)
            continue;
        // проверяем разницу в цене на минимальный порог
        if (PriceRange(order.getPrice(), price).abs() < sEqualRate)
            return true;
    }
    return false;
}

void TraderManager::onCloseCandle(const BinanceKlineData& data) {
    trace("candle closed o(%f) h(%f) l(%f) c(%f)\n", data.priceOpen, data.priceHigh, data.priceLow, data.priceClose);
}

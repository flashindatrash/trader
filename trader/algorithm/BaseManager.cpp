#include "BaseManager.hpp"
#include "Logger.hpp"
#include "proxy/BinanceKlines.hpp"
#include "exchanger/wrapper/CandlestickContainer.hpp"

BaseManager::BaseManager(OrderManager& orders)
    : _orders(orders)
{
}

bool BaseManager::init(const Symbol& symbol) {
    // устанавливаем враппер свеч
    _candlesticks = SKlines().get(symbol);
    if (_candlesticks == nullptr)
        Logger::error("BaseManager can't init candlestick container");

    _candlesticks->addListener(std::bind(&BaseManager::onCloseCandle, this, std::placeholders::_1));
    return true;
}

#pragma once

#include <global.hpp>

class CandlestickContainer;
class BinanceKlineData;
class OrderManager;
class Symbol;

class BaseManager
{
public: // methods
    BaseManager(OrderManager& orders);

    virtual bool init(const Symbol& symbol);

protected: // vars
    OrderManager& _orders;
    CandlestickContainer* _candlesticks = nullptr;

    // свеча закрылась
    virtual void onCloseCandle(const BinanceKlineData& data) {};
};


#pragma once

class ChartWrapper;
class CandlestickWrapper;
class OrderManager;
class Symbol;

class BaseManager
{
public: // methods
    BaseManager(OrderManager& orders);

    virtual bool init(const Symbol& symbol);
    virtual void tick(const Symbol& symbol) {}

protected: // vars
    OrderManager& _orders;
    ChartWrapper* _candlesticks = nullptr;

    // свеча закрылась
    virtual void onCloseCandle(const CandlestickWrapper& wrapper) {};
};


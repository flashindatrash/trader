#pragma once

#include "exchanger/base/Storage.hpp"

namespace core {
    class Config;
}

class OrderWrapper;
struct OrderRequest;
enum ChartInterval : unsigned int;

class ExchangerController {
public: // static
    static ExchangerController* create();

public: // virtual
    virtual ~ExchangerController() = default;
    virtual bool init(const core::Config& config) = 0;
    virtual void run() = 0;
    virtual void tick(time_t now) = 0;

    virtual bool getSymbolInfo(Storage::Type_pair& container) const = 0;
    virtual bool getAllPrices(Storage::Type_price& container) const = 0;
    virtual bool getBalances(Storage::Type_balance& container) const = 0;
    virtual bool getOrders(BookWrapper& wrapper) const = 0;
    virtual bool getChart(ChartWrapper& wrapper, ChartInterval interval) const = 0;

    virtual void connectPrices(Storage::Type_price& container) = 0;
    virtual void connectBalances(Storage::Type_balance& container) = 0;
    virtual void connectOrders(Storage::Type_book& container) = 0;
    virtual void connectStats(Storage::Type_stat& container) = 0;
    virtual void connectChart(Storage::Type_chart& container) = 0;

    virtual void listenStats(CandlestickWrapper& wrapper);
    virtual void listenChart(ChartWrapper& wrapper, ChartInterval interval);

    virtual const OrderWrapper* createOrder(const OrderRequest& request) = 0;

protected: // methods
    ExchangerController() = default;
};


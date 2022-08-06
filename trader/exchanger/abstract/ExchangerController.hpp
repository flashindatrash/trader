#pragma once

#include "exchanger/base/Storage.hpp"
#include "exchanger/base/Decimal.hpp"
#include <cmath>

namespace core {
    class Config;
}

class OrderWrapper;
struct ChartRequest;
struct OrderRequest;
struct StakingRequest;
enum ChartInterval : unsigned int;

class ExchangerController {
public: // static
    static ExchangerController* create();

public: // virtual
    virtual ~ExchangerController() = default;
    virtual bool init(const core::Config& config) = 0;
    virtual void run() = 0;
    virtual void tick(time_t now) = 0;

    virtual void connectPrices(Storage::Type_price& container) = 0;
    virtual void connectBalances(Storage::Type_balance& container) = 0;
    virtual void connectCharts(Storage::Type_chart& container) = 0;

    virtual bool loadPairs(Storage::Type_pair& container) const = 0;
    virtual bool loadPrices(Storage::Type_price& container) const = 0;
	virtual bool loadStakings(Storage::Type_staking& container) const = 0;
    virtual bool loadPrice(PriceWrapper& container) const = 0;
    virtual bool loadBalances(Storage::Type_balance& container) const = 0;
    virtual bool loadOrders(BookWrapper& container) const = 0;
    virtual bool loadStats(CandlestickWrapper& container) const = 0;
    virtual bool loadCharts(ChartWrapper& container, ChartRequest& request) const = 0;

    virtual void listenCharts(ChartWrapper& container, ChartInterval interval) = 0;
    virtual void listenTicker(PriceWrapper& container) = 0;
    virtual void unlistenTicker(PriceWrapper& container) = 0;

    virtual const OrderWrapper* createOrder(BookWrapper& container, OrderRequest& request) = 0;
    virtual bool stake(StakingWrapper& container, StakingRequest& request) = 0;

    virtual bool updateStaking(StakingWrapper& container) const = 0;
    virtual Decimal roundQuantity(Decimal quantity, const std::string& symbol, double(*fn)(double)) const = 0;
    virtual double fee() const = 0;

protected: // methods
    ExchangerController() = default;
};


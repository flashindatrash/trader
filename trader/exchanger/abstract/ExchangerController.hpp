#pragma once

#include <memory>
#include "exchanger/base/Storage.hpp"
#include "exchanger/base/Decimal.hpp"

namespace core {
    class Config;
}

class OrderWrapper;
struct ChartRequest;
struct OrderRequest;
enum ChartInterval : unsigned int;

class ExchangerController {
public: // static
    static std::unique_ptr<ExchangerController> create();
    static std::unique_ptr<ExchangerController> create(const core::Config& config);

public: // virtual
    virtual ~ExchangerController() = default;
    virtual bool init(const core::Config& config) = 0;
    virtual void run() = 0;
    virtual void tick(time_t now) = 0;

    virtual void connectPrices(Storage::Type_price& container) = 0;
    virtual void connectBalances(Storage::Type_balance& container) = 0;
    virtual void connectCharts(Storage::Type_chart& container) = 0;

    virtual bool loadPairs(Storage::Type_pair& container) const = 0;
    virtual bool loadCharts(ChartWrapper& container, ChartRequest& request) const = 0;

    virtual void listenCharts(ChartWrapper& container, ChartInterval interval) = 0;
    virtual void listenTicker(PriceWrapper& container) = 0;

    virtual const OrderWrapper* createOrder(BookWrapper& container, OrderRequest& request) = 0;

    virtual Decimal roundQuantity(Decimal quantity, const std::string& symbol) const = 0;
    virtual double fee() const = 0;

protected: // methods
    ExchangerController() = default;
};

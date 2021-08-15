#include "StatusManager.hpp"
#include "Logger.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "OrderManager.hpp"


StatusManager::StatusManager(OrderManager& orders)
    : BaseManager(orders)
{
}

void StatusManager::tick(const Symbol& symbol) {
    double baseBalance = symbol.baseAsset().getBalance();
    double quoteBalance = symbol.quoteAsset().getBalance();

    double change = getChange();
    Price current = symbol.getPrice();
    std::string timeline = getTimline(current);

    std::string formatBaseBalance = "%." + std::to_string(getZerosAfterDot(baseBalance) + 1) + "f";
    std::string formatQuoteBalance = "%." + std::to_string(getZerosAfterDot(quoteBalance) + 1) + "f";
    std::string formatPrice = "%." + std::to_string(getZerosAfterDot(current) + 3) + "f";
    std::string formatChange = change > 0.0 ? "+%.3f" : "%.3f%%";
    std::string format = "%s (" + formatBaseBalance + ") - %s (" + formatQuoteBalance + ") " + formatPrice + " (" + formatChange + ")" + " %s";
    Logger::title(format.c_str(),
                  symbol.baseAsset().c_str(),
                  baseBalance,
                  symbol.quoteAsset().c_str(),
                  quoteBalance,
                  current,
                  change,
                  timeline.c_str());
}

std::string StatusManager::getTimline(double current) {
    auto positions = _orders.getPositions();
    std::sort(positions.begin(), positions.end(), [](const BinanceOrderData& l, const BinanceOrderData& r) {
        return l.getPrice() < r.getPrice();
    });

    std::string timeline = "";
    bool current_embeded = false;
    for (const BinanceOrderData& position : positions) {
        if (not current_embeded && current < position.getPrice()) {
            timeline += "|";
            current_embeded = true;
        }
        if (SideEnum(position.side) == SideEnum::Buy)
            timeline += "+";
        else
            timeline += "-";
    }

    if (not current_embeded)
        timeline += "|";
    return timeline;
}

double StatusManager::getChange() {
    if (not _candlesticks->klines().empty()) {
        CandlestickWrapper* last = _candlesticks->klines().back();
        return PriceRange(last->priceOpen(), last->priceClose()).change() * 100.0;
    }
    return 0.0;
}

int StatusManager::getZerosAfterDot(double num) {
    if (num == 0.0)
        return 0;

    int count = 0;
    while (num < 1.0) {
        ++count;
        num*=10.0;
    }
    return count;
};

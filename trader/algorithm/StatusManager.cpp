#include "StatusManager.hpp"
#include "OrderManager.hpp"
#include "Logger.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "util/NumberUtil.hpp"

StatusManager::StatusManager(OrderManager& orders)
    : BaseManager(orders)
{
}

void StatusManager::tick(const Symbol& symbol) {
    double baseBalance = symbol.baseAsset().getBalance();
    double quoteBalance = symbol.quoteAsset().getBalance();

    double change = getChange();
    Price current = symbol.getPrice();
    std::string timeline = "";
    double losses = 0.0;

    bool current_embeded = false;
    for (const OrderWrapper* position : _orders.getPositions()) {
        Price price = position->price();

        if (not current_embeded && current < price) {
            timeline += "|";
            current_embeded = true;
        }
        if (position->side() == OrderSide::Buy)
            timeline += "+";
        else if (position->side() == OrderSide::Sell)
            timeline += "-";

        bool sell_loss = position->side() == OrderSide::Sell && current > price;
        bool buy_loss = position->side() == OrderSide::Buy && current < price;

        if (sell_loss || buy_loss)
            losses += std::abs(current - price) * position->quantity();
    }

    if (not current_embeded)
        timeline += "|";

    std::string formatBaseBalance = "%." + std::to_string(util::zeros_after_dot(baseBalance) + 1) + "f";
    std::string formatQuoteBalance = "%." + std::to_string(util::zeros_after_dot(quoteBalance) + 1) + "f";
    std::string formatLosses = "%." + std::to_string(util::zeros_after_dot(losses) + 1) + "f";
    std::string formatPrice = "%." + std::to_string(util::zeros_after_dot(current) + 3) + "f";
    std::string formatChange = change > 0.0 ? "+%.3f" : "%.3f";
    std::string format = "%s (" + formatBaseBalance + ") - %s (" + formatQuoteBalance + ") " + formatPrice + " (" + formatChange + "%%)" + " losses -" + formatLosses + " %s";
    Logger::title(format.c_str(),
                  symbol.baseAsset().c_str(),
                  baseBalance,
                  symbol.quoteAsset().c_str(),
                  quoteBalance,
                  current,
                  change,
                  losses,
                  timeline.c_str());
}

double StatusManager::getChange() {
    if (not Exchanger().chart()->get().empty()) {
        const CandlestickWrapper* last = Exchanger().chart()->last();
        return util::change(last->priceOpen(), last->priceClose()) * 100.0;
    }
    return 0.0;
}

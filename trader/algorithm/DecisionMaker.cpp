#include "Logger.hpp"
#include "DecisionMaker.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/base/ExchangerTypes.hpp"
#include "exchanger/base/Symbol.hpp"

DecisionMaker::DecisionMaker(const Symbol& symbol)
    : _symbol(symbol)
{
}

double DecisionMaker::factor(const SideEnum& side, int based_on) const {
    if (side == SideEnum::Invalid)
        return 0.0;

    double result = 1.0;

    if (has(based_on, DayChange)) {
        // увеличиваем/понижаем рейтинг при отрицательном дневном росте/падении
        const CandlestickWrapper* stat = Exchanger().stat();
        if (stat->priceOpen() > 0.0) {
            PriceRange range(stat->priceOpen(), _symbol.getPrice());
            result *= 1.0 + range.change() * (side == SideEnum::Buy ? 1 : -1);
        }
    }

    if (has(based_on, Balance)) {
        // понижаем рейтинг на сделки, при балансе ассета < 50%
        static double sBalanceThreshold = 0.5;
        double baseQty = _symbol.getPrice(_symbol.baseAsset().getBalance());
        double quoteQty = _symbol.quoteAsset().getBalance();
        result *= std::abs((side == SideEnum::Sell ? baseQty : quoteQty) / ((baseQty + quoteQty) * sBalanceThreshold));
    }

    return result;
}

bool DecisionMaker::has(int mask, BasedOn value) const {
    return 0 != (mask & value);
}

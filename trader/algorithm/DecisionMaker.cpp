#include "Logger.hpp"
#include "DecisionMaker.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

DecisionMaker::DecisionMaker(const Symbol& symbol, const std::vector<const OrderWrapper*>& positions)
    : _symbol(symbol)
    , _positions(positions)
{
}

double DecisionMaker::factor(const OrderSide& side, int based_on) const {
    if (side == OrderSide::Invalid)
        return 0.0;

    double result = 1.0;

    if (has(based_on, DayChange)) {
        // увеличиваем/понижаем рейтинг при отрицательном дневном росте/падении
        const CandlestickWrapper* stat = Exchanger().stat();
        if (stat->priceOpen() > 0.0) {
            Change change = util::change(stat->priceOpen(), _symbol.getPrice());
            result *= 1.0 + change * (side == OrderSide::Buy ? 1 : -1);
        }
    }

    if (has(based_on, Balance)) {
        Quantity baseQty = _symbol.getPrice(_symbol.baseAsset().getBalance());
        Quantity quoteQty = _symbol.quoteAsset().getBalance();
        // мы должны иметь валюту для закрытия сделки
        for (const OrderWrapper* position : _positions) {
            if (position->side() == OrderSide::Buy)
                baseQty -= position->quantity();
            else if (position->side() == OrderSide::Sell)
                quoteQty -= position->price() * position->quantity();
        }
        baseQty = std::max(baseQty, 1.0);
        quoteQty = std::max(quoteQty, 1.0);
        if (baseQty > 0.0 || quoteQty > 0.0)
            result *= std::abs((side == OrderSide::Sell ? baseQty : quoteQty) / (baseQty + quoteQty));
        else
            result *= 0;
    }

    return result;
}

bool DecisionMaker::has(int mask, BasedOn value) const {
    return 0 != (mask & value);
}

#include "DecisionMaker.hpp"
#include "Logger.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

using namespace scalping;

DecisionMaker::DecisionMaker(const std::vector<const OrderWrapper*>& positions)
    : _positions(positions)
{
}

double DecisionMaker::factor(const OrderRequest& request, int based_on) const {
    if (request.side == OrderSide::Invalid)
        return 0.0;

    double result = 1.0;

    if (has(based_on, DayChange)) {
        // увеличиваем/понижаем рейтинг при отрицательном дневном росте/падении
        const CandlestickWrapper* stat = Exchanger().stat(request.symbol);
        if (stat != nullptr && stat->priceOpen() > 0.0) {
            Change change = OrderUtil::change(stat->priceOpen(), request.symbol.price());
            result *= 1.0 + change * (request.side == OrderSide::Buy ? 1 : -1);
        }
    }

    if (has(based_on, Balance)) {
        Quantity baseQty = request.symbol.price() * request.symbol.baseAsset().balance();
        Quantity quoteQty = request.symbol.quoteAsset().balance();
        Quantity sumQty = baseQty + quoteQty;
        // мы должны иметь валюту для закрытия сделки
        for (const OrderWrapper* position : _positions) {
            Change change = std::abs(OrderUtil::change(position->price(), request.symbol.price()));
            // коэффициент влияния, если сделка была не так далеко, то держать для нее баланс важнее
            double k = std::max(0.1 - change, 0.0);
            if (position->side() == OrderSide::Buy)
                baseQty -= position->baseQuantity() * k;
            else if (position->side() == OrderSide::Sell)
                quoteQty -= position->price() * position->baseQuantity() * k;
        }
        baseQty = std::max(baseQty, 0.0);
        quoteQty = std::max(quoteQty, 0.0);

        result *= std::abs((request.side == OrderSide::Sell ? baseQty : quoteQty) / sumQty);
    }

    return result;
}

bool DecisionMaker::has(int mask, BasedOn value) const {
    return 0 != (mask & value);
}

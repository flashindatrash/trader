#include "proxy/TraderTime.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/binance/response/BinanceSymbolData.hpp"
#include "util/PriceUtil.hpp"

double util::get_min_quantity(const Symbol& symbol) {
    const BinanceSymbolData& info = symbol.getInfo();
    const BinanceSymbolData::MinNotional& min_notional = info.minNotional;
    const BinanceSymbolData::LotSize& lot_size = info.lotSize;

    double price_avg = symbol.getPrice();
    if (const PriceWrapper* history = Exchanger().price(symbol))
        price_avg = history->getPriceAverage(min_notional.avgPriceMins * TraderTime::sMinute);

    return std::max(lot_size.minQty, min_notional.minNotional / price_avg);
}

double util::ceil_quantity(const Symbol& symbol, double quantity) {
    const BinanceSymbolData& info = symbol.getInfo();

    if (info.lotSize.stepSize > 0.0) {
        double steps = 0.0;
        while (steps < quantity)
            steps += info.lotSize.stepSize;
        quantity = steps;
    }

    return quantity;
}

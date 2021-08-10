#include "proxy/BinanceTime.hpp"
#include "proxy/BinancePrices.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "wrapper/PriceSymbol.hpp"
#include "data/BinanceSymbolData.hpp"
#include "util/PriceUtil.hpp"

double util::get_min_quantity(const TradeSymbol& symbol) {
    const BinanceSymbolData& info = symbol.getInfo();
    const BinanceSymbolData::MinNotional& min_notional = info.minNotional;
    const BinanceSymbolData::LotSize& lot_size = info.lotSize;

    double price_avg = symbol.getPrice();
    if (const PriceSymbol* history = SPrices().getPrice(symbol))
        price_avg = history->getPriceAverage(min_notional.avgPriceMins * BinanceTime::sMinute);

    return std::max(lot_size.minQty, min_notional.minNotional / price_avg);
}

double util::ceil_quantity(const TradeSymbol& symbol, double quantity) {
    const BinanceSymbolData& info = symbol.getInfo();

    if (info.lotSize.stepSize > 0.0) {
        double steps = 0.0;
        while (steps < quantity)
            steps += info.lotSize.stepSize;
        quantity = steps;
    }

    return quantity;
}

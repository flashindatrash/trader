#include "proxy/TraderTime.hpp"
#include "proxy/BinanceExchangeInfo.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/binance/response/BinanceSymbolData.hpp"
#include "util/PriceUtil.hpp"
#include "util/NumberUtil.hpp"

double util::get_min_quantity(const Symbol& symbol) {
    const BinanceSymbolData& info = SExchangeInfo().getSymbolInfo(symbol.id());
    const BinanceSymbolData::MinNotional& min_notional = info.minNotional;
    const BinanceSymbolData::LotSize& lot_size = info.lotSize;

    Price price_avg = symbol.getPrice();
    if (const PriceWrapper* history = Exchanger().price(symbol.id()))
        price_avg = history->getPriceAverage(min_notional.avgPriceMins * TraderTime::sMinute);

    return std::max(lot_size.minQty, min_notional.minNotional / price_avg);
}

double util::ceil_quantity(const Symbol& symbol, double quantity) {
    const BinanceSymbolData& info = SExchangeInfo().getSymbolInfo(symbol.id());

    if (info.lotSize.stepSize > 0.0)
        return util::ceil_steps(quantity, info.lotSize.stepSize);

    return quantity;
}

#include "Runner.hpp"

#include <utility>
#include "Context.hpp"
#include "core/Time.hpp"
#include "core/Logger.hpp"
#include "base/Settings.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

using namespace pair;

Runner* Runner::create() {
    auto* runner = new Runner();
    return runner;
}

bool Runner::start(const Settings& settings) {
    _chart = Exchanger().chart(settings.symbol);
    if (settings.isBackTest()) {
        PriceWrapper price;

        const std::vector<CandlestickWrapper*>& candlesticks = _chart->get();
        for (auto it = candlesticks.cbegin(); it < candlesticks.cend(); ++it) {
            // в режиме BackTest цена одна и равна закрытой свечи
            const CandlestickWrapper* candlestick = *it;
            price.set(candlestick->priceClose());

            Ticker ticker;
            ticker.bestBidPrice = ticker.bestAskPrice = price.get();
            price.set(ticker);

            // переопределим время логов временем свечи
            Logger::setTime(candlestick->timeOpen() / 1000);

            setContext(Context(it, price));
        }
        return false;
    }

    Time().onTick.connect(std::bind(&Runner::tick, this, std::placeholders::_1));
    return true;
}

void Runner::setCallback(Callback::Fn callback) {
    _dispatcher.connect(std::move(callback));
}

void Runner::setContext(const Context& context) {
    Context::current = &context;
    _dispatcher.emmit(nullptr);
}

void Runner::tick(time_t ms) {
    if (_chart->get().empty())
        return;

    setContext(Context(_chart->get().cend() - 1, *Exchanger().price(_chart->id())));
}
#include "Runner.hpp"

#include <utility>
#include "Settings.hpp"
#include "Context.hpp"
#include "Time.hpp"
#include "Logger.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

NS_USE

Runner* Runner::create() {
    auto* runner = new Runner();
    return runner;
}

void Runner::start(const Settings& settings) {
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

            Context context(it, price);
            _dispatcher.emmit(context);
        }
        return;
    }

    _active = true;
    Time().onTick.connect(std::bind(&Runner::tick, this, std::placeholders::_1));
}

void Runner::setCallback(Callback::Fn callback) {
    _dispatcher.connect(std::move(callback));
}

void Runner::tick(time_t ms) {
    if (_chart->get().empty())
        return;

    Context context(_chart->get().cend() - 1, *Exchanger().price(_chart->id()));
    _dispatcher.emmit(context);
}

bool Runner::isActive() const {
    return _active;
}

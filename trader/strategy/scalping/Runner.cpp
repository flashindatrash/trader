#include "Runner.hpp"

#include <utility>
#include "Settings.hpp"
#include "Context.hpp"
#include "Time.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"

NS_USE

Runner* Runner::create() {
    auto* runner = new Runner();
    return runner;
}

void Runner::start(const Settings& settings) {
    _active = true;
    _chart = Exchanger().chart(settings.symbol);
    Time().onTick.connect(std::bind(&Runner::tick, this, std::placeholders::_1));
}

void Runner::setCallback(Callback::Fn callback) {
    _dispatcher.connect(std::move(callback));
}

void Runner::tick(time_t ms) {
    const CandlestickWrapper* candlestick = _chart->last();
    if (candlestick != nullptr)
        _dispatcher.emmit(candlestick);
}

bool Runner::isActive() const {
    return _active;
}

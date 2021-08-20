#include "Runner.hpp"
#include "Settings.hpp"
#include "proxy/Exchanger.hpp"
#include "proxy/Time.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

NS_USE

Runner* Runner::create(const Settings& settings) {
    Runner* runner = new Runner(settings.pair, settings.test);
    return runner;
}

Runner::Runner(const Symbol& symbol, bool test)
    : _active(true)
    , _symbol(symbol)
    , _test(test)
{
}

void Runner::run() {
    if (_test) {
        Exchanger().loadCharts(_symbol, ChartInterval::m15);
        if (const ChartWrapper* chart = Exchanger().chart(_symbol)) {
            for (const CandlestickWrapper* candlestick : chart->get()) {
                tick(candlestick->timeOpen());
            }
        }
        _active = false;
    } else {
        Exchanger().listenCharts(_symbol, ChartInterval::m15);
        Time().onTick.connect(std::bind(&Runner::tick, this, std::placeholders::_1));
    }
}

void Runner::tick(time_t ms) {
    onTick.emmit(ms);
}

bool Runner::isRunning() const {
    return _active;
}

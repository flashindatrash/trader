#include "Runner.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "exchanger/Exchanger.hpp"
#include "Time.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

NS_USE

Runner* Runner::create() {
    Runner* runner = new Runner();
    return runner;
}

bool Runner::init(const Settings& settings) {
    if (settings.test) {
        if (not Exchanger().loadCharts(_symbol, ChartInterval::m15))
            return false;

        if (const ChartWrapper* chart = Exchanger().chart(_symbol)) {
            for (const CandlestickWrapper* candlestick : chart->get()) {
                Context context;
                context.time = candlestick->timeClose();
                context.candlestick = candlestick;
                dispatch(context);
            }
        }
        _active = false;
    } else {
        Exchanger().listenCharts(_symbol, ChartInterval::m15);
        Time().onTick.connect(std::bind(&Runner::tick, this, std::placeholders::_1));
    }
    return true;
}

void Runner::setCallback(Callback::Fn callback) {
    _dispatcher.connect(callback);
}

void Runner::dispatch(const Context& context) {
    if (not context.isValid())
        return;

    _dispatcher.emmit(context);
}

void Runner::tick(time_t ms) {
    Context context;
    context.time = ms;
    context.candlestick = Exchanger().chart(_symbol)->last();
    dispatch(context);
}

bool Runner::isRunning() const {
    return _active;
}

#include "Runner.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Time.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

NS_USE

Runner* Runner::create() {
    Runner* runner = new Runner();
    return runner;
}

bool Runner::init(const Settings& settings) {
    _chart = Exchanger().chart(settings.symbol);
    if (settings.test) {
        if (not Exchanger().loadCharts(_chart->id(), ChartInterval::m15))
            return false;

        for (const CandlestickWrapper* candlestick : _chart->get()) {
            Context context;
            Time().set(candlestick->timeClose());
            context.candlestick = candlestick;
            dispatch(context);
        }
    } else {
        _active = true;
        Time().onTick.connect(std::bind(&Runner::tick, this, std::placeholders::_1));
    }

    return isRunning();
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
    context.candlestick = _chart->last();
    if (context.candlestick == nullptr)
        return;

    dispatch(context);
}

bool Runner::isRunning() const {
    return _active;
}

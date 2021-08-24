#include "Runner.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Time.hpp"
#include "database/Database.hpp"
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
    if (_chart == nullptr)
        return false;

    if (settings.test) {
        DB().permissions(db::Database::Read);

        if (not Exchanger().loadCharts(_chart->id(), ChartInterval::m15))
            return false;

        for (const CandlestickWrapper* candlestick : _chart->get()) {
            Context context;
            context.time = candlestick->timeClose();
            context.candlestick = candlestick;
            dispatch(context);
        }
    } else {
        _active = true;
        Exchanger().listenCharts(_chart->id(), ChartInterval::m15);
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
    context.candlestick = _chart->last();
    dispatch(context);
}

bool Runner::isRunning() const {
    return _active;
}

#include "binacpp.h"
#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "data/BinanceErrorData.hpp"
#include <chrono>

const time_t BinanceTime::sSecond = 1000;
const time_t BinanceTime::sMinute = 60 * sSecond;
const time_t BinanceTime::sHour = 60 * sMinute;
const time_t BinanceTime::sDay = 24 * sHour;

void BinanceTime::init()
{
    Json::Value result;
    BinaCPP::get_serverTime(result);

    BinanceErrorData error(result);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return;
    }

    if (not result["serverTime"] || not result["serverTime"].isInt64()) {
        trace("%s\n", result.toStyledString().c_str());
        logic_error("invalid server time");
        return;
    }

    time_t server_time = result["serverTime"].asInt64();

    using namespace std::chrono;
    time_t local_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    if (std::abs(server_time - local_time) > 1000) {
        // todo: поддержать разницу во времени
        logic_error("time desynchronizated\n");
    }
}

void BinanceTime::tick() {
    invoke(getCurrent());
}

time_t BinanceTime::getCurrent() const {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

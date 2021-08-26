#include "Time.hpp"
#include <chrono>

const time_t Timer::sSecond = 1000;
const time_t Timer::sMinute = 60 * sSecond;
const time_t Timer::sHour = 60 * sMinute;
const time_t Timer::sDay = 24 * sHour;

void Timer::tick() {
    onTick.emmit(ms());
}

void Timer::set(time_t ms) {
    _local = ms;
}

time_t Timer::ms() const {
    if (_local != 0.0)
        return _local;

    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

time_t Timer::sec() const {
    if (_local != 0.0)
        return _local/1000;

    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

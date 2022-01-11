#include "Time.hpp"
#include <chrono>

const time_t Timer::sSecond = 1000;
const time_t Timer::sMinute = 60 * sSecond;
const time_t Timer::sHour = 60 * sMinute;
const time_t Timer::sDay = 24 * sHour;

void Timer::tick() {
    onTick.emmit(ms());
}

time_t Timer::ms() const {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

time_t Timer::sec() const {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

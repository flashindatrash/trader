#include "TraderTime.hpp"
#include <chrono>

const time_t TraderTime::sSecond = 1000;
const time_t TraderTime::sMinute = 60 * sSecond;
const time_t TraderTime::sHour = 60 * sMinute;
const time_t TraderTime::sDay = 24 * sHour;

void TraderTime::tick() {
    onTick.emmit(ms());
}

time_t TraderTime::ms() const {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

#include "App.hpp"
#include <chrono>
#include <thread>

core::App::App(const Config& config)
    : _config(config)
{
}

void core::App::sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

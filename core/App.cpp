#include "App.hpp"
#include <chrono>
#include <thread>

using namespace core;

App::App(Config config)
    : _config(std::move(config))
{
}

void App::sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

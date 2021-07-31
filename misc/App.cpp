#include "App.hpp"
#include <chrono>
#include <thread>

using namespace Core;

App::App()
{

}

void App::run()
{
    init();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        tick();
    }
}

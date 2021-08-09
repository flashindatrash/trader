#pragma once

#include <string>

namespace core {
    class Config;
}

namespace tools {
    int get_price(const core::Config& cfg, std::string symbol);
};


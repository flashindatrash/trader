#pragma once

#include "Defines.hpp"

namespace core {
    class Config;
}

NS_BEGIN
class Runner;
class Algorithm;
NS_END

class Strategy {
public: // static
    static Strategy* create(const core::Config& config);

public: // methods
    ~Strategy();

    bool init(const core::Config& config);
    bool isRunning() const;

protected: // methods
    NS::Runner* _runner = nullptr;
    NS::Algorithm* _algorithm = nullptr;

protected: // methods
    Strategy() = default;
};

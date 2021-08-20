#pragma once

namespace core {
    class Config;
}

class Strategy {
public: // static
    static Strategy* create(const core::Config& config);

public: // methods
    virtual ~Strategy() = default;

    virtual bool init(const core::Config& config) = 0;
    virtual bool isRunning() const = 0;

protected: // methods
    Strategy() = default;

};


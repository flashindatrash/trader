#pragma once

namespace core {
    class Config;
}

class Strategy {
public: // static
    static Strategy* create();

public: // methods
    virtual ~Strategy() = default;

    virtual bool init(const core::Config& config) = 0;

protected: // methods
    Strategy() = default;

};


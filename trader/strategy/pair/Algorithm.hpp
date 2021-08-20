#pragma once

#include "Defines.hpp"

NS_BEGIN
class Settings;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    void execute();

protected: // methods
    Algorithm() = default;
};
NS_END

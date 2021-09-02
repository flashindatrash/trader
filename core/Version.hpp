#pragma once

#include <string>

namespace core {
struct Version {
public: // constructors
    Version() = default;
    Version(int major, int minor, int patch = 0);

    std::string toString() const;

public: // vars
    int major = 0, minor = 0, patch = 0;
};

}


#pragma once

#include <string>

namespace core {
struct Version {
public: // constructors
    Version() = default;
    Version(int major, int minor, int patch = 0);

    std::string toString() const;

public: // vars
    int _major = 0, _minor = 0, _patch = 0;
};

}


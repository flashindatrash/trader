#pragma once

namespace core {

struct Version
{
public: // constructors
    Version() = default;
    Version(int major, int minor, int patch = 0);

public: // vars
    int major, minor, patch = 0;
};

}


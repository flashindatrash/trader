#include "Version.hpp"

using namespace core;

Version::Version(int major, int minor, int patch)
    : _major(major)
    , _minor(minor)
    , _patch(patch)
{
}

std::string Version::toString() const {
    return std::to_string(_major) + "." + std::to_string(_minor) + "." + std::to_string(_patch);
}

#include "Version.hpp"

using namespace core;

Version::Version(int major, int minor, int patch)
    : major(major)
    , minor(minor)
    , patch(patch)
{
}

std::string Version::toString() const {
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}
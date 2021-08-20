#pragma once

#include <string>

namespace database {
class OrderWrapper {
    typedef std::string Id;

public: // methods
    static OrderWrapper* create(Id id);

protected: // methods
    OrderWrapper() = default;
};
}

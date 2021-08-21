#pragma once

#include "Defines.hpp"
#include "database/BaseWrapper.hpp"

NS_BEGIN
class Position : database::BaseWrapper {
    typedef int Id;

public: // static
    static Position* get(const Id id);

public: // methods
    Price price() const;
    void setPrice(const Price value);

protected: // methods
    Position(const Id id);
};
NS_END

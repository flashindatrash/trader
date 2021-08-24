#pragma once

#include "Defines.hpp"
#include "database/Array.hpp"

NS_BEGIN
class Positions {
public: // static
    static Positions* create(const Symbol& pair);

protected: // methods
    Positions(const Symbol& pair);

protected: // vars
    db::ArrayObject _array;
};
NS_END


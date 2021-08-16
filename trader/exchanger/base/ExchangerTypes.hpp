#pragma once

#include <string>

typedef double Price;
typedef double Change;

namespace Changes {
    bool empty(Change change);
    bool equal(Change left, Change right);
}

class PriceRange {

public:  // methods
    PriceRange() = default;
    PriceRange(Change change);
    PriceRange(Price left, Price right);

    Change change() const;
    Change abs() const;

protected: // vars
    Change _change;
};

#pragma once

typedef double Price;
typedef double Change;

namespace Changes {
    bool equal(Change left, Change right);
}

class PriceRange {

public:  // methods
    PriceRange() {}
    PriceRange(Change change);
    PriceRange(Price left, Price right);

    Change change() const;
    Change abs() const;

protected: // vars
    Change _change;
};


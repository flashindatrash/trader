#pragma once

typedef double Price;
typedef double Change;

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


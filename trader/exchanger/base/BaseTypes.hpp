#pragma once

typedef double Price;
typedef double Change;
typedef double Quantity;

namespace util {
    Change change(const Price& left, const Price& right);
};

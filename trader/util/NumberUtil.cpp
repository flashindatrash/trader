#include "NumberUtil.hpp"

int util::getZerosAfterDot(double num) {
    if (num == 0.0)
        return 0;

    int count = 0;
    while (num < 1.0) {
        ++count;
        num*=10.0;
    }
    return count;
};

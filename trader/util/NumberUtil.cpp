#include "NumberUtil.hpp"

unsigned int util::zeros_after_dot(double num) {
    if (num == 0.0)
        return 0;

    unsigned int count = 0;
    while (num < 1.0) {
        ++count;
        num*=10.0;
    }
    return count;
};

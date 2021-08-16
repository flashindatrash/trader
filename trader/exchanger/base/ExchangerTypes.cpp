#include "ExchangerTypes.hpp"

Change util::change(const Price& left, const Price& right) {
    return (right - left) / left;
}

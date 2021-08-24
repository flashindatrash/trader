#include "Positions.hpp"

NS_USE

Positions* Positions::create(const Symbol& pair) {
    Positions* positions = new Positions(pair);
    return positions;
}

Positions::Positions(const Symbol& pair)
    : _array("positions:" + pair.id())
{
}

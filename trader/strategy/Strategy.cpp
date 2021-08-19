#include "Strategy.hpp"
#include "long_short/Strategy.hpp"

Strategy* Strategy::create() {
    Strategy* strategy = new longshort::LongShortStrategy();
    return strategy;
}

#include "Strategy.hpp"
#include "long_short/LongShortStrategy.hpp"

Strategy* Strategy::create() {
    Strategy* strategy = new TraderAlgorithm();
    return strategy;
}

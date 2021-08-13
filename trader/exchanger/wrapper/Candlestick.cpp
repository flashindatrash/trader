#include "exchanger/wrapper/Candlestick.hpp"
#include <algorithm>

Candlestick::Candlestick(Price open, Price high, Price low, Price close)
    : open(open), high(high), low(low), close(close)
{
}

Price Candlestick::bodyLen() const {
    return std::abs(open - close);
}

Price Candlestick::wickLen() const {
    return high - std::max(open, close);
}

Price Candlestick::tailLen() const {
    return std::min(open, close) - low;
}

bool Candlestick::isBullish(const Candlestick& candlestick) {
    return candlestick.open < candlestick.close;
}

bool Candlestick::isBearish(const Candlestick& candlestick) {
    return candlestick.open > candlestick.close;
}

bool Candlestick::isHammerLike(const Candlestick& candlestick) {
    return candlestick.tailLen() > (candlestick.bodyLen() * 2) &&
           candlestick.wickLen() < candlestick.bodyLen();
}

bool Candlestick::isInvertedHammerLike(const Candlestick& candlestick) {
    return candlestick.wickLen() > (candlestick.bodyLen() * 2) &&
           candlestick.tailLen() < candlestick.bodyLen();
}

bool Candlestick::isEngulfed(const Candlestick& shortest, const Candlestick& longest) {
    return shortest.bodyLen() < longest.bodyLen();
}

bool Candlestick::isGap(const Candlestick& lowest, const Candlestick& upmost) {
    return std::max(lowest.open, lowest.close) < std::min(upmost.open, upmost.close);
}

bool Candlestick::isGapUp(const Candlestick& previous, const Candlestick& current) {
    return isGap(previous, current);
}

bool Candlestick::isGapDown(const Candlestick& previous, const Candlestick& current) {
    return isGap(current, previous);
}

bool Candlestick::isHammer(const Candlestick& candlestick) {
    return isBullish(candlestick) &&
           isHammerLike(candlestick);
}

bool Candlestick::isInvertedHammer(const Candlestick& candlestick) {
    return isBearish(candlestick) &&
           isInvertedHammerLike(candlestick);
}

bool Candlestick::isHangingMan(const Candlestick& previous, const Candlestick& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isGapUp(previous, current) &&
           isHammerLike(current);
}

bool Candlestick::isShootingStar(const Candlestick& previous, const Candlestick& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isGapUp(previous, current) &&
           isInvertedHammerLike(current);
}

bool Candlestick::isBullishEngulfing(const Candlestick& previous, const Candlestick& current) {
    return isBearish(previous) &&
           isBullish(current) &&
           isEngulfed(previous, current);
}

bool Candlestick::isBearishEngulfing(const Candlestick& previous, const Candlestick& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isEngulfed(previous, current);
}

bool Candlestick::isBullishHarami(const Candlestick& previous, const Candlestick& current) {
    return isBearish(previous) &&
           isBullish(current) &&
           isEngulfed(current, previous);
}

bool Candlestick::isBearishHarami(const Candlestick& previous, const Candlestick& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isEngulfed(current, previous);
}

bool Candlestick::isBullishKicker(const Candlestick& previous, const Candlestick& current) {
    return isBearish(previous) &&
           isBullish(current) &&
           isGapUp(previous, current);
}

bool Candlestick::isBearishKicker(const Candlestick& previous, const Candlestick& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isGapDown(previous, current);
}

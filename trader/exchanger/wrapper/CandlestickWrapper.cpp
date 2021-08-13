#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include <algorithm>

CandlestickWrapper::CandlestickWrapper(Price open, Price high, Price low, Price close)
    : open(open), high(high), low(low), close(close)
{
}

Price CandlestickWrapper::bodyLen() const {
    return std::abs(open - close);
}

Price CandlestickWrapper::wickLen() const {
    return high - std::max(open, close);
}

Price CandlestickWrapper::tailLen() const {
    return std::min(open, close) - low;
}

bool CandlestickWrapper::isBullish(const CandlestickWrapper& candlestick) {
    return candlestick.open < candlestick.close;
}

bool CandlestickWrapper::isBearish(const CandlestickWrapper& candlestick) {
    return candlestick.open > candlestick.close;
}

bool CandlestickWrapper::isHammerLike(const CandlestickWrapper& candlestick) {
    return candlestick.tailLen() > (candlestick.bodyLen() * 2) &&
           candlestick.wickLen() < candlestick.bodyLen();
}

bool CandlestickWrapper::isInvertedHammerLike(const CandlestickWrapper& candlestick) {
    return candlestick.wickLen() > (candlestick.bodyLen() * 2) &&
           candlestick.tailLen() < candlestick.bodyLen();
}

bool CandlestickWrapper::isEngulfed(const CandlestickWrapper& shortest, const CandlestickWrapper& longest) {
    return shortest.bodyLen() < longest.bodyLen();
}

bool CandlestickWrapper::isGap(const CandlestickWrapper& lowest, const CandlestickWrapper& upmost) {
    return std::max(lowest.open, lowest.close) < std::min(upmost.open, upmost.close);
}

bool CandlestickWrapper::isGapUp(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isGap(previous, current);
}

bool CandlestickWrapper::isGapDown(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isGap(current, previous);
}

bool CandlestickWrapper::isHammer(const CandlestickWrapper& candlestick) {
    return isBullish(candlestick) &&
           isHammerLike(candlestick);
}

bool CandlestickWrapper::isInvertedHammer(const CandlestickWrapper& candlestick) {
    return isBearish(candlestick) &&
           isInvertedHammerLike(candlestick);
}

bool CandlestickWrapper::isHangingMan(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isGapUp(previous, current) &&
           isHammerLike(current);
}

bool CandlestickWrapper::isShootingStar(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isGapUp(previous, current) &&
           isInvertedHammerLike(current);
}

bool CandlestickWrapper::isBullishEngulfing(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isBearish(previous) &&
           isBullish(current) &&
           isEngulfed(previous, current);
}

bool CandlestickWrapper::isBearishEngulfing(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isEngulfed(previous, current);
}

bool CandlestickWrapper::isBullishHarami(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isBearish(previous) &&
           isBullish(current) &&
           isEngulfed(current, previous);
}

bool CandlestickWrapper::isBearishHarami(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isEngulfed(current, previous);
}

bool CandlestickWrapper::isBullishKicker(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isBearish(previous) &&
           isBullish(current) &&
           isGapUp(previous, current);
}

bool CandlestickWrapper::isBearishKicker(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isBullish(previous) &&
           isBearish(current) &&
           isGapDown(previous, current);
}

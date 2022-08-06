#include "CandlestickPattern.hpp"
#include "CandlestickWrapper.hpp"

bool CandlestickPattern::isHammerLike(const CandlestickWrapper& candlestick) {
    return candlestick.tailLen() > (candlestick.bodyLen() * 2.0) &&
           candlestick.wickLen() < candlestick.bodyLen();
}

bool CandlestickPattern::isInvertedHammerLike(const CandlestickWrapper& candlestick) {
    return candlestick.wickLen() > (candlestick.bodyLen() * 2.0) &&
           candlestick.tailLen() < candlestick.bodyLen();
}

bool CandlestickPattern::isEngulfed(const CandlestickWrapper& shortest, const CandlestickWrapper& longest) {
    return shortest.bodyLen() < longest.bodyLen();
}

bool CandlestickPattern::isGap(const CandlestickWrapper& lowest, const CandlestickWrapper& upmost) {
    return std::max(lowest.priceOpen(), lowest.priceClose()) < std::min(upmost.priceOpen(), upmost.priceClose());
}

bool CandlestickPattern::isGapUp(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isGap(previous, current);
}

bool CandlestickPattern::isGapDown(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return isGap(current, previous);
}

bool CandlestickPattern::isHammer(const CandlestickWrapper& candlestick) {
    return candlestick.isBullish() &&
           isHammerLike(candlestick);
}

bool CandlestickPattern::isInvertedHammer(const CandlestickWrapper& candlestick) {
    return candlestick.isBearish() &&
           isInvertedHammerLike(candlestick);
}

bool CandlestickPattern::isHangingMan(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return previous.isBullish() &&
           current.isBearish() &&
           isGapUp(previous, current) &&
           isHammerLike(current);
}

bool CandlestickPattern::isShootingStar(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return previous.isBullish() &&
           current.isBearish() &&
           isGapUp(previous, current) &&
           isInvertedHammerLike(current);
}

bool CandlestickPattern::isBullishEngulfing(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return previous.isBearish() &&
           current.isBullish() &&
           isEngulfed(previous, current);
}

bool CandlestickPattern::isBearishEngulfing(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return previous.isBullish() &&
           current.isBearish() &&
           isEngulfed(previous, current);
}

bool CandlestickPattern::isBullishHarami(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return previous.isBearish() &&
           current.isBullish() &&
           isEngulfed(current, previous);
}

bool CandlestickPattern::isBearishHarami(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return previous.isBullish() &&
           current.isBearish() &&
           isEngulfed(current, previous);
}

bool CandlestickPattern::isBullishKicker(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return previous.isBearish() &&
           current.isBullish() &&
           isGapUp(previous, current);
}

bool CandlestickPattern::isBearishKicker(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    return previous.isBullish() &&
           current.isBearish() &&
           isGapDown(previous, current);
}

CandlestickPattern::Pattern CandlestickPattern::find(const CandlestickWrapper& previous, const CandlestickWrapper& current) {
    if (CandlestickPattern::isHammer(current)) return Hammer;
    if (CandlestickPattern::isInvertedHammer(current)) return InvertedHammer;
    if (CandlestickPattern::isHangingMan(previous, current)) return HangingMan;
    if (CandlestickPattern::isShootingStar(previous, current)) return ShootingStar;
    if (CandlestickPattern::isBullishEngulfing(previous, current)) return BullishEngulfing;
    if (CandlestickPattern::isBearishEngulfing(previous, current)) return BearishEngulfing;
    if (CandlestickPattern::isBullishHarami(previous, current)) return BullishHarami;
    if (CandlestickPattern::isBearishHarami(previous, current)) return BearishHarami;
    if (CandlestickPattern::isBullishKicker(previous, current)) return BullishKicker;
    if (CandlestickPattern::isBearishKicker(previous, current)) return BearishKicker;
    return None;
}

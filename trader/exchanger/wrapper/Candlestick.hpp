#pragma once

#include "exchanger/wrapper/PriceBase.hpp"

class Candlestick {
protected: // static
    static bool isEngulfed(const Candlestick& shortest, const Candlestick& longest);
    static bool isGap(const Candlestick& lowest, const Candlestick& upmost);
    static bool isGapUp(const Candlestick& previous, const Candlestick& current);
    static bool isGapDown(const Candlestick& previous, const Candlestick& current);

    static bool isBullish(const Candlestick& candlestick);
    static bool isBearish(const Candlestick& candlestick);
    static bool isHammerLike(const Candlestick& candlestick);
    static bool isInvertedHammerLike(const Candlestick& candlestick);

public: // methods
    Candlestick(Price open, Price high, Price low, Price close);

protected: // methods
    Price bodyLen() const;
    Price wickLen() const;
    Price tailLen() const;

public: // pattern detection.
    static bool isHammer(const Candlestick& candlestick);
    static bool isInvertedHammer(const Candlestick& candlestick);
    static bool isHangingMan(const Candlestick& previous, const Candlestick& current);
    static bool isShootingStar(const Candlestick& previous, const Candlestick& current);
    static bool isBullishEngulfing(const Candlestick& previous, const Candlestick& current);
    static bool isBearishEngulfing(const Candlestick& previous, const Candlestick& current);
    static bool isBullishHarami(const Candlestick& previous, const Candlestick& current);
    static bool isBearishHarami(const Candlestick& previous, const Candlestick& current);
    static bool isBullishKicker(const Candlestick& previous, const Candlestick& current);
    static bool isBearishKicker(const Candlestick& previous, const Candlestick& current);

public: // vars
    Price open, high, low, close = 0.0;
};


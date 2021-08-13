#pragma once

#include "exchanger/wrapper/PriceWrapper.hpp"

class CandlestickWrapper {
public: // enum
    enum Pattern {
        None,
        Hammer,
        InvertedHammer,
        HangingMan,
        ShootingStar,
        BullishEngulfing,
        BearishEngulfing,
        BullishHarami,
        BearishHarami,
        BullishKicker,
        BearishKicker
    };

protected: // static
    static bool isEngulfed(const CandlestickWrapper& shortest, const CandlestickWrapper& longest);
    static bool isGap(const CandlestickWrapper& lowest, const CandlestickWrapper& upmost);
    static bool isGapUp(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isGapDown(const CandlestickWrapper& previous, const CandlestickWrapper& current);

    static bool isBullish(const CandlestickWrapper& candlestick);
    static bool isBearish(const CandlestickWrapper& candlestick);
    static bool isHammerLike(const CandlestickWrapper& candlestick);
    static bool isInvertedHammerLike(const CandlestickWrapper& candlestick);

public: // methods
    CandlestickWrapper(Price open, Price high, Price low, Price close);

    Pattern getPattern(const CandlestickWrapper& previous) const;

protected: // methods
    Price bodyLen() const;
    Price wickLen() const;
    Price tailLen() const;

public: // pattern detection.
    static bool isHammer(const CandlestickWrapper& candlestick);
    static bool isInvertedHammer(const CandlestickWrapper& candlestick);
    static bool isHangingMan(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isShootingStar(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBullishEngulfing(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    // A bearish engulfing pattern is a technical chart pattern that signals lower prices to come
    static bool isBearishEngulfing(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    // A bullish harami is a candlestick chart indicator suggesting that a bearish trend may be coming to end. Some investors may look at a bullish harami as a good sign that they should enter a long position on an asset.
    static bool isBullishHarami(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    // A bearish harami is a two bar Japanese candlestick pattern that suggests prices may soon reverse to the downside
    static bool isBearishHarami(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBullishKicker(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBearishKicker(const CandlestickWrapper& previous, const CandlestickWrapper& current);

public: // vars
    Price open, high, low, close = 0.0;
};


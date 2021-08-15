#pragma once

class CandlestickWrapper;

class CandlestickPattern {
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

protected: // helpers
    static bool isEngulfed(const CandlestickWrapper& shortest, const CandlestickWrapper& longest);
    static bool isGap(const CandlestickWrapper& lowest, const CandlestickWrapper& upmost);
    static bool isGapUp(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isGapDown(const CandlestickWrapper& previous, const CandlestickWrapper& current);

    static bool isBullish(const CandlestickWrapper& candlestick);
    static bool isBearish(const CandlestickWrapper& candlestick);
    static bool isHammerLike(const CandlestickWrapper& candlestick);
    static bool isInvertedHammerLike(const CandlestickWrapper& candlestick);

protected: // pattern detection.
    static bool isHammer(const CandlestickWrapper& candlestick);
    static bool isInvertedHammer(const CandlestickWrapper& candlestick);
    static bool isHangingMan(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isShootingStar(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBullishEngulfing(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBearishEngulfing(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBullishHarami(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBearishHarami(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBullishKicker(const CandlestickWrapper& previous, const CandlestickWrapper& current);
    static bool isBearishKicker(const CandlestickWrapper& previous, const CandlestickWrapper& current);

public: // static
    static Pattern find(const CandlestickWrapper& previous, const CandlestickWrapper& current);
};


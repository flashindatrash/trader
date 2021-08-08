#include "wrapper/TradeSymbol.hpp"
#include "algorithm/DecisionMaker.hpp"

DecisionMaker::DecisionMaker(const TradeSymbol& symbol)
    : _symbol(symbol)
{
}

bool DecisionMaker::make(double change, double min, double max, int based_on) {
    if (based_on == 0)
        return false;

    if (has(based_on, Balane)) {
        double baseQty = _symbol.getPrice(_symbol.baseAsset().getBalance());
        double quoteQty = _symbol.quoteAsset().getBalance();
        double sumQty = (baseQty + quoteQty) * 0.5;
        double base = std::min(1.0, baseQty / sumQty);
        double quote = std::min(1.0, quoteQty / sumQty);

        double expected = min + (change > 0 ? 1.0 - base : 1.0 - quote) * (max - min);
        if (std::abs(change) < expected)
            return false;
    }

    return true;
}

bool DecisionMaker::has(int mask, BasedOn value) {
    return 0 != (mask & value);
}

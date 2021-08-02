#include "BinanceAlgorithm.hpp"

#include "Logger.hpp"
#include "BinanceSymbol.hpp"
#include "BinanceBook.hpp"
#include "BinanceOrders.hpp"
#include "data/BinanceOrderData.hpp"
#include "data/BinanceBookData.hpp"

BinanceAlgorithm::BinanceAlgorithm() {
}

bool BinanceAlgorithm::init() {
    return true;
}

void BinanceAlgorithm::connect(const BinanceSymbol& symbol) {
    _symbol = symbol;

    SBinanceBook().addListener(std::bind(&BinanceAlgorithm::onBookData, this, std::placeholders::_1));
}

void BinanceAlgorithm::onBookData(const BinanceBookData& data) {
    // на какой цене купить/продать
    double sellAt = _last_price + _last_price * _rate_up;
    double buyAt = _last_price - _last_price * _rate_down;

    std::string side;
    double priceAt = 0.0;
    double price = 0.0;
    bool create = false;

    if (_last_side == "SELL") {
        side = "BUY";
        priceAt = buyAt;
        price = data.bestBidPrice;
        create = price <= priceAt;
    } else {
        side = "SELL";
        priceAt = sellAt;
        price = data.bestAskPrice;
        create = price >= priceAt;
    }

    if (create)
    {
        BinanceOrderData result = SBinanceOrders().createOrder(_symbol, side, _lot);
        log("%s %f %s for %f\n", side.c_str(), result.quantity, _symbol.baseAsset().c_str(), result.cummulativeQuoteQty);
        _last_price = price;
        _last_side = side;
    } else {
        log("hold to %s for %f (current %f)\n", side.c_str(), priceAt, price);
    }
}

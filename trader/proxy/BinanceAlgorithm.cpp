#include "BinanceAlgorithm.hpp"

#include "Logger.hpp"
#include "BinanceSymbol.hpp"
#include "BinanceBook.hpp"
#include "BinanceOrders.hpp"
#include "data/BinanceBookData.hpp"

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */

void BinanceAlgorithm::init(const BinanceSymbol& symbol) {
    _symbol = symbol;

    double balance = symbol.getPrice() * symbol.baseAsset().getBalance() + symbol.quoteAsset().getBalance();
    log("%sbalance %f %s%s\n", GREEN, balance, symbol.quoteAsset().c_str(), RESET);

    _orderHistory = SBinanceOrders().getAllOrders(symbol);
    if (_orderHistory.empty()) {
        log("order history is empty\n");
        throw 42;
    }

    SBinanceBook().addListener(std::bind(&BinanceAlgorithm::onBookData, this, std::placeholders::_1));
}

void BinanceAlgorithm::onBookData(const BinanceBookData& data) {
    std::cout << '\r';

    // информация о последнем ордере
    const BinanceOrderData& last = _orderHistory.back();
    double last_price = last.cummulativeQuoteQty / last.quantity;

    // на какой цене купить/продать
    double sellAt = last_price + last_price * _rate_up;
    double buyAt = last_price - last_price * _rate_down;

    std::string side;
    double priceAt = 0.0;
    double price = 0.0;
    bool create = false;

    if (last.side == "SELL") {
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
        if (result.orderId == 0)
            return;

        log("\n%s %f %s for %f\n", side.c_str(), result.quantity, _symbol.baseAsset().c_str(), result.cummulativeQuoteQty);
        _orderHistory.push_back(result);
    } else {
        log("hold to %s for %f (current %f)", side.c_str(), priceAt, price);
    }
}

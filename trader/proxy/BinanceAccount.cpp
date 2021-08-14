#include "binacpp.h"
#include "binacpp_websocket.h"
#include "Logger.hpp"
#include "Config.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinanceAccount.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/binance/response/BinanceBalanceData.hpp"
#include "exchanger/binance/response/BinanceOrderData.hpp"
#include "exchanger/binance/response/BinanceErrorData.hpp"

void BinanceAccount::init() {
    Json::Value result;

    BinaCPP::start_userDataStream(result);
    BinanceErrorData error(result);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return;
    }

    if (!result["listenKey"] || !result["listenKey"].isString()) {
        trace("%s\n", result.toStyledString().c_str());
        logic_error("can't get listenKey for stream account");
        return;
    }

    _listen_key = result["listenKey"].asString();

    std::string ws_path = std::string("/ws/");
    ws_path.append(_listen_key);

    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceAccount::handle, this, std::placeholders::_1), ws_path.c_str());

    _ping_time = STime().getCurrent();
    STime().addListener(std::bind(&BinanceAccount::tick, this, std::placeholders::_1));
}

int BinanceAccount::handle(Json::Value& json) {
    std::string action = json["e"].asString();
    if (action  == "executionReport") {
        std::string executionType = json["x"].asString();
        if (executionType == "NEW") {
            BinanceOrderData order(json, true);
            if (order.isRejected())
                logic_error(json["r"].asString().c_str());
        }
    } else if (action == "outboundAccountPosition") {
        for (uint i = 0; i < json["B"].size(); ++i) {
            BinanceBalanceData data(json["B"][i], true);
            Exchanger().balance_mutable(data.asset)->set(data.free, data.locked);
        }
    }

    return 0;
}

void BinanceAccount::tick(time_t now) {
    // Keepalive a user data stream to prevent a time out.
    // User data streams will close after 60 minutes.
    // It's recommended to send a ping about every 30 minutes
    if (now < _ping_time + BinanceTime::sMinute * 30)
        return;

    BinaCPP::keep_userDataStream(_listen_key.c_str());
    _ping_time = now;
}

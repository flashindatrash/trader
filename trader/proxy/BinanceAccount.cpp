#include "BinanceAccount.hpp"

#include "binacpp.h"
#include "binacpp_websocket.h"
#include "Logger.hpp"
#include "BinanceConfig.hpp"
#include "data/BinanceStreamBalanceData.hpp"

BinanceAccount::BinanceAccount() {
}

bool BinanceAccount::init() {
    Json::Value result;
    BinaCPP::get_account( BINANCE_RECV_WINDOW , result );

    const Json::Value& balances = result["balances"];
    if (not balances.isArray()) {
        log("%s\n", result.toStyledString().c_str());
        return false;
    }

    if (result["accountType"].asString() != "SPOT") {
        log("%s\n", result.toStyledString().c_str());
        return false;
    }

    for (uint i = 0; i < balances.size(); ++i) {
        const Json::Value& data = balances[i];

        std::string asset = data["asset"].asString();
        double free = atof(data["free"].asString().c_str());
        double locked = atof(data["locked"].asString().c_str());
        setBalance(asset, free, locked);
    }


    return true;
}

void BinanceAccount::connect() {
    Json::Value result;
    BinaCPP::start_userDataStream(result);

    if (!result["listenKey"] || !result["listenKey"].isString()) {
        log("error: BinanceAccount::BinanceAccount, listenKey null\n");
        return;
    }

    std::string ws_path = std::string("/ws/");
    ws_path.append( result["listenKey"].asString() );

    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceAccount::handle, this, std::placeholders::_1), ws_path.c_str());
}

int BinanceAccount::handle(Json::Value& json) {
    std::string action = json["e"].asString();
    if ( action  == "executionReport" ) {
        std::string executionType = json["x"].asString();
        std::string orderStatus   = json["X"].asString();
        std::string reason        = json["r"].asString();
        std::string symbol 	      = json["s"].asString();
        std::string side          = json["S"].asString();
        std::string orderType     = json["o"].asString();
        std::string orderId       = json["i"].asString();
        std::string price 	      = json["p"].asString();
        std::string qty           = json["q"].asString();

        if ( executionType  == "NEW" ) {
            if ( orderStatus == "REJECTED" ) {
                log("Order Failed! Reason: %s\n", reason.c_str());
            }
            log("%s %s %s %s(%s) %s %s\n", symbol.c_str(), side.c_str(), orderType.c_str(), orderId.c_str(), orderStatus.c_str(), price.c_str(), qty.c_str());
            return 0;
        }
        log("%s %s %s %s %s\n", symbol.c_str(), side.c_str(), executionType.c_str(), orderType.c_str(), orderId.c_str());


    } else if ( action == "outboundAccountInfo" ) {
        for (uint i = 0; i < json["B"].size(); ++i) {
            BinanceStreamBalanceData balance(json["B"][i]);
            setBalance(balance.asset, balance.free, balance.locked);
        }
    }

    return 0;
}

double BinanceAccount::getBalance(const std::string &asset) const {
    if (_balance.find(asset) == _balance.end())
        return 0.0;

    return _balance.at(asset);
}

void BinanceAccount::setBalance(const std::string& asset, double free, double locked) {
    _balance[asset] = free;
}

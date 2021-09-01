#include "BinanceWebsocket.hpp"
#include "binacpp_websocket.h"
#include <json/json.h>
#include <functional>
#include "Logger.hpp"
#include "response/BinanceErrorData.hpp"

BinanceWebsocket* BinanceWebsocket::create() {
    BinanceWebsocket* websocket = new BinanceWebsocket();
    return websocket;
}

void BinanceWebsocket::setPath(std::string path) {
    _path = path;
}

void BinanceWebsocket::setCallback(SignalT::Fn callback) {
    _callback.connect(callback);
}

bool BinanceWebsocket::isConnected() const {
    return _connected;
}

const std::string& BinanceWebsocket::path() const {
    return _path;
}

void BinanceWebsocket::connect() {
    if (_connected)
        return;

    _connected = true;
    std::string endpoint = "/ws/" + _path;
    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceWebsocket::handler, this, std::placeholders::_1), endpoint.c_str());
}

int BinanceWebsocket::handler(Json::Value& json) {
    BinanceErrorData error(json, "BinanceWebsocket::handler(" + _path + ")");
    if (error.has()) {
        Logger::info(error.msg.c_str());
        if (error.code == BinanceErrorData::DISCONNECTED)
            _connected = false;
        return 0;
    }

    _callback.emmit(json);
    return 0;
}

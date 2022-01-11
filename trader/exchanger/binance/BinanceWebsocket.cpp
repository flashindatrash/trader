#include "BinanceWebsocket.hpp"
#include "binacpp_websocket.h"
#include <json/json.h>
#include <functional>
#include <utility>
#include "core/Logger.hpp"
#include "response/BinanceErrorData.hpp"

BinanceWebsocket* BinanceWebsocket::create() {
    auto* websocket = new BinanceWebsocket();
    return websocket;
}

void BinanceWebsocket::setPath(std::string path) {
    _path = std::move(path);
}

void BinanceWebsocket::setCallback(SignalT::Fn callback) {
    _callback.connect(std::move(callback));
}

void BinanceWebsocket::setType(Type type) {
    _type = type;
}

bool BinanceWebsocket::isConnected() const {
    return _connected;
}

const std::string& BinanceWebsocket::path() const {
    return _path;
}

const BinanceWebsocket::Type& BinanceWebsocket::type() const {
    return _type;
}

void BinanceWebsocket::connect() {
    if (_connected)
        return;

    std::string endpoint = "/ws/" + _path;
    if (BinaCPP_websocket::connect_endpoint(std::bind(&BinanceWebsocket::handler, this, std::placeholders::_1), endpoint.c_str()))
        _connected = true;
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
    return 1;
}

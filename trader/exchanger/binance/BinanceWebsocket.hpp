#pragma once

#include <string>
#include "Signal.hpp"

namespace Json {
    class Value;
}

class BinanceWebsocket {
public: // methods
    BinanceWebsocket(std::string path);

    void connect();

protected: // methods
    int handler(Json::Value& json);

public: // signals
    Signal<Json::Value&> onData;

public: // vars
    bool connected = false;
    std::string path = "";
};


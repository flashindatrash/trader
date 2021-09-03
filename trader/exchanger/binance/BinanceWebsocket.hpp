#pragma once

#include <string>
#include "Signal.hpp"

namespace Json {
    class Value;
}

class BinanceWebsocket {
public: // static

    typedef Json::Value SignalV;
    typedef Signal<SignalV> SignalT;

    static BinanceWebsocket* create();

public: // methods
    void setPath(std::string path);
    void setCallback(SignalT::Fn callback);

    void connect();

    bool isConnected() const;
    const std::string& path() const;

protected: // methods
    BinanceWebsocket() = default;

    int handler(SignalV& json);

protected: // vars
    SignalT _callback;
    bool _connected = false;
    std::string _path;
};


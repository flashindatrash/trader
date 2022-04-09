#pragma once

#include <string>
#include "core/Signal.hpp"

namespace Json {
    class Value;
}

class BinanceWebsocket {
public: // static
    enum Type {
        Default,
        UserStream
    };

    typedef Json::Value SignalV;
    typedef Signal<SignalV> SignalT;

    static BinanceWebsocket* create();

public: // methods
    void setPath(std::string path);
    void setCallback(SignalT::Fn callback);
    void setType(Type type);

    bool connect();
    bool disconnect();

    bool isConnected() const;
    const std::string& path() const;
    const Type& type() const;

protected: // methods
    BinanceWebsocket() = default;

    int handler(SignalV& json);

protected: // vars
    void* _connection = nullptr;
    SignalT _callback;
    Type _type = Default;
    std::string _path;
};


#pragma once

#include "Proxy.hpp"
#include <functional>
#include <vector>

namespace Json {
    class Value;
}

class BinanceSymbol;
class BinanceBookData;

class BinanceBook : public Core::Proxy<BinanceBook>
{
    typedef std::function<void(const BinanceBookData&)> Fn;

public: // methods
    BinanceBook();

    void connect(const BinanceSymbol& symbol);
    int handle(Json::Value& json);

    void addListener(Fn listener);

protected: // vars
    std::vector<Fn> _listeners;
};

#define SBinanceBook() BinanceBook::getInstance()

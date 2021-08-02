#pragma once

#include "App.hpp"

#include <string>

#include "BinanceSymbol.hpp"

class TraderApp : public Core::App
{
public: // static
    static TraderApp* create();

protected: // methods
    TraderApp();

    void init() override;
};


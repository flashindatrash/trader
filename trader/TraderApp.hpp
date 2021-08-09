#pragma once

#include "App.hpp"

class TradeSymbol;

class TraderApp : public Core::App
{
public: // static
    static TraderApp* create();

public: // methods
    void run(const TradeSymbol& symbol);

protected: // methods
    TraderApp();

};


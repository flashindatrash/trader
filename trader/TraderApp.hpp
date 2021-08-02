#pragma once

#include "App.hpp"

class TraderApp : public Core::App
{
public: // static
    static TraderApp* create();

protected: // methods
    TraderApp();

    void init() override;
};


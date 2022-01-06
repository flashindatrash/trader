//
// Created by Вадим Проскурин on 06.01.2022.
//

#pragma once

#include "App.hpp"

class BaseApp : public core::App {
public: // methods
    BaseApp(core::Config config, core::Version version);

    virtual int run() = 0;
};



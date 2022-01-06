//
// Created by Вадим Проскурин on 06.01.2022.
//

#pragma once

#include "BaseApp.hpp"

class Symbol;

class ListingApp : public BaseApp {
public: // static
    static ListingApp* create(const core::Config& config);

public: // methods
    int run() override;

protected: // methods
    explicit ListingApp(const core::Config& config);
    ~ListingApp() override;

    void onSymbolAdded(const Symbol& symbol);

};



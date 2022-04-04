//
// Created by Вадим Проскурин on 15.01.2022.
//

#pragma once

#include "exchanger/base/Symbol.hpp"

namespace listing {

class SymbolUpdater {
public: // static
    typedef std::vector<Symbol> Data;

public: // methods
    SymbolUpdater() = default;
    ~SymbolUpdater();

    bool request();

    const Data& vector() const;

protected: // methods

    void connect();
    void disconnect();

    void found(const Symbol& symbol);

protected: // vars
    std::vector<Symbol> _symbols;

    size_t _connector = -1;
};

}
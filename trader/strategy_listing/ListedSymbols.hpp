//
// Created by Вадим Проскурин on 15.01.2022.
//

#pragma once

#include "exchanger/base/Symbol.hpp"

namespace listing {

class ListedSymbols {
public: // static
    typedef std::vector<Symbol> Data;

    enum Status : unsigned int {
        Ok,
        Failed,
        Empty
    };

    static ListedSymbols find();

public: // methods
    ~ListedSymbols();

    Status status() const;
    const Data& vector() const;

protected: // methods
    ListedSymbols() = default;

    void connect();
    void disconnect();

    void proceed();

    void found(const Symbol& symbol);

protected: // vars
    std::vector<Symbol> _symbols;

    Status _status = Empty;
    size_t _connector = -1;
};

}
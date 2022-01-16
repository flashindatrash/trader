//
// Created by Вадим Проскурин on 16.01.2022.
//

#pragma once

class Symbol;
class Position;
namespace core {
    class Config;
}

namespace listing {

class Algorithm {
public: // static
    static Algorithm* create(const core::Config& config);

public: // methods
    ~Algorithm();

    bool init(const Symbol& symbol);
    bool execute();

protected: // methods
    explicit Algorithm(const core::Config& config);

protected: // vars
    const core::Config& _config;

    Position* _position = nullptr;

};

}



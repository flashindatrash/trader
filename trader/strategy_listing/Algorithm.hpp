//
// Created by Вадим Проскурин on 16.01.2022.
//

#pragma once

class Symbol;
class Position;
struct Settings;

namespace listing {

class Algorithm {
public: // static
    static Algorithm* create(const Settings& config);

public: // methods
    ~Algorithm();

    bool init(const Symbol& symbol);
    bool execute();

protected: // methods
    explicit Algorithm(const Settings& config);

    bool tryOpen();
    bool tryClose();

protected: // vars
    const Settings& _settings;

    Position* _position = nullptr;

};

}



//
// Created by Вадим Проскурин on 16.01.2022.
//

#pragma once

class Symbol;
class Position;
class Settings;
struct OrderRequest;

namespace listing {

class Algorithm {
public: // static
    static Algorithm* create(const Settings& config);

public: // methods
    virtual ~Algorithm();

    bool init(const Symbol& symbol);
    bool execute();

protected: // methods
    explicit Algorithm(const Settings& config);

    bool tryOpen();
    bool tryClose();

    bool createOrder(OrderRequest& request, Position& result) const;

protected: // vars
    const Settings& _settings;

    Position* _position = nullptr;

};

}



//
// Created by Вадим Проскурин on 22.11.2021.
//


#pragma once

#include "database/Object.hpp"

namespace protocol {

class Stats : public db::Object {
public: // methods
    void setProfit(double value);
    double profit() const;

    void setChange(double value);
    double change() const;

protected: // methods
    Stats(db::Key key);
};

}



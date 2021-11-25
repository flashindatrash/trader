//
// Created by Вадим Проскурин on 28.08.2021.
//

#pragma once

#include "database/Object.hpp"

namespace protocol {

class Trader : public db::Object {
public: // methods
    Trader(const std::string& username, const std::string& symbol);

    void setProfit(double value);
    double profit() const;

    void setChange(double value);
    double change() const;

    void setEarnBase(double value);
    double earnBase() const;

    void setEarnQuote(double value);
    double earnQuote() const;

    std::string version() const;
    void setVersion(std::string value);
};

}

//
// Created by Вадим Проскурин on 28.08.2021.
//

#pragma once

#include "Stats.hpp"

namespace protocol {

class Pair : public Stats {
public: // methods
    Pair(const std::string& username, const std::string& symbol);

    void setEarnBase(double value);
    double earnBase() const;

    void setEarnQuote(double value);
    double earnQuote() const;

    std::string version() const;
    void setVersion(std::string value);
};

}

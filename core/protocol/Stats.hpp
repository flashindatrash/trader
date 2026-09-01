//
// Created by Вадим Проскурин on 28.08.2021.
//

#pragma once

#include "database/Object.hpp"

namespace protocol {

class Stats : public db::Object {
private: // static
    static std::string key(const std::string& username, const std::string& symbol);

public: // methods
    Stats(const std::string& username, const std::string& symbol);

    void setProfit(int64_t value);
    int64_t profit() const;

    void setEarnBase(int64_t value);
    int64_t earnBase() const;

    void setEarnQuote(int64_t value);
    int64_t earnQuote() const;

    void setVersion(std::string value);
    std::string version() const;
};

}

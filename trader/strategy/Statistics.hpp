//
// Created by Вадим Проскурин on 28.08.2021.
//

#pragma once

#include "Defines.hpp"
#include "database/Object.hpp"

NS_BEGIN
class Position;
struct Report;

class Statistics : public db::Object {
public: // static
    static Statistics* create(const db::Key& key);

public: // methods
    void report(const Report& report);

    std::string version() const;
    Quantity profit() const;
    Quantity earnBase() const;
    Quantity earnQuote() const;

    void setVersion(std::string value);

protected: // methods
    Statistics(const db::Key& key);
};
NS_END


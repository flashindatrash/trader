//
// Created by Вадим Проскурин on 28.08.2021.
//


#pragma once

#include "Defines.hpp"
#include "database/Object.hpp"

NS_BEGIN
class Statistics : public db::Object {
public: // static
    static Statistics* create(const db::Key& key);

public: // methods
    Quantity addProfit(Quantity profit);

    std::string version();
    void setVersion(std::string value);

protected: // methods
    Statistics(const db::Key& key);
};
NS_END


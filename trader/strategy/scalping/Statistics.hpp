//
// Created by Вадим Проскурин on 28.08.2021.
//


#pragma once

#include "Defines.hpp"
#include "database/Object.hpp"

NS_BEGIN
class Statistics : public db::Object {
public: // static
    static Statistics* create(const Symbol& pair, bool sync);

public: // methods
    Quantity addProfit(Quantity profit);

protected: // methods
    Statistics(const db::Key& key, bool sync);

protected: // vars
    const bool _sync;
};
NS_END


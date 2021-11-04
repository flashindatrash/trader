//
// Created by Вадим Проскурин on 05.11.2021.
//

#pragma once

#include "Defines.hpp"
#include "database/Array.hpp"

NS_BEGIN
class Events : public db::Array {
public: // static
    static Events* create(const db::Key& key);

public: // methods
    bool send(const char* fmt, ...);

protected: // methods
    Events(const db::Key& key);
};
NS_END



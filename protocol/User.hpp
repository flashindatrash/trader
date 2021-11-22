//
// Created by Вадим Проскурин on 05.11.2021.
//

#pragma once

#include "database/Array.hpp"
#include "Stats.hpp"

namespace protocol {

class User : public Stats {
public: // methods
    User(const std::string& username);

    void setId(int value);
    int id() const;

    std::string name() const;
};

class Users : public db::ArrayAbstract<User> {
    typedef db::ArrayAbstract<User> BaseClass;

public: // static
    static Users::Predicate byName(const std::string& name);
    static Users::Predicate byId(int id);

public: // methods
    Users(const db::Key& key);

protected: // methods
    bool proceed_push(User& value) const override;
};

}
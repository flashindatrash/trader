//
// Created by Вадим Проскурин on 05.11.2021.
//

#pragma once

#include "database/Array.hpp"

namespace protocol {

class User : public db::Object {
public: // methods
    User(const std::string& username);

    std::string name() const;

    void setId(int value);
    int id() const;

    void setProfit(double value);
    double profit() const;

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
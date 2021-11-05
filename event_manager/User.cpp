//
// Created by Вадим Проскурин on 05.11.2021.
//

#include "User.hpp"

static const char* FIELD_ID = "id";

User::User(const db::Key& key)
    : db::Object(key)
{
    load();
}

void User::setId(int value) {
    set(FIELD_ID, value);
}

int User::id() const {
    return get(FIELD_ID).asInt();
}

std::string User::name() const {
    return _key;
}

Users::Users(const db::Key& key)
    : BaseClass(key)
{
}

bool Users::proceed_push(User& value) const {
    return BaseClass::proceed_sync() && value.save();
}

Users::Predicate Users::byName(const std::string& name) {
    return [name](const User& user) {
        return user.name() == name;
    };
}

Users::Predicate Users::byId(int id) {
    return [id](const User& user) {
        return user.id() == id;
    };
}

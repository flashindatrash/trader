//
// Created by Вадим Проскурин on 05.11.2021.
//

#include "User.hpp"

using namespace protocol;

static const char* FIELD_ID = "id";
static const char* FIELD_PROFIT = "profit";

User::User(const std::string& username)
    : db::Object(username)
{
}

std::string User::name() const {
    return _key;
}

void User::setId(int value) {
    set(FIELD_ID, value);
}

int User::id() const {
    return get(FIELD_ID).asInt();
}

void User::setProfit(double value) {
    inc(FIELD_PROFIT, value);
}

double User::profit() const {
    return get(FIELD_PROFIT).asDouble();
}

Users::Users(const db::Key& key)
    : BaseClass(key)
{
    load();
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

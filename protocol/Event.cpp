//
// Created by Вадим Проскурин on 21.11.2021.
//

#include "Event.hpp"

#include <utility>
#include "Storage.hpp"
#include "database/Database.hpp"

using namespace protocol;

std::string Event::key(const std::string& username) {
    return Storage::key(username, "events");
}

bool Event::add(const std::string& username, const std::string& text) {
    return not text.empty() && DB().rpush(key(username), text) > 0;
}

std::vector<Event> Event::get(const std::string& username) {
    std::string storage = key(username);

    std::vector<Event> events;
    db::VectorValues values = DB().lrange(storage);
    for (const db::Value& value : values)
        events.emplace_back(value.asString());

    if (not values.empty())
        DB().del(storage);

    return events;
}

Event::Event(std::string text)
    : _text(std::move(text))
{
}

const std::string& Event::text() const {
    return _text;
}
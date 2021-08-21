#include "Book.hpp"
#include "Position.hpp"
#include <global.hpp>

NS_USE

Book::Book(const Id id)
    : database::Array("book:" + id)
{
    setId(id);
    onAdded.connect(std::bind(&Book::handleAdded, this, std::placeholders::_1));
    onRemoved.connect(std::bind(&Book::handleRemoved, this, std::placeholders::_1));
}

Book::~Book() {
    for (auto& it : _positions)
        SAFE_DELETE(it.second);
    _positions.clear();
}

bool Book::add(Position* position) {
    if (position == nullptr)
        return false;

    // already exist
    const Id id = position->id();
    if (has(id))
        return false;

    // try to save object
    if (not position->flush())
        return false;

    // cache and push
    _positions[id] = position;
    push(id);
    return true;
}

Book::Vector Book::positions() const {
    Book::Vector vector;
    for (auto& it : _positions)
        vector.push_back(it.second);
    return vector;
}

void Book::handleAdded(const database::Value& value) {
    Id id = value.asString();
    auto it = _positions.find(id);
    if (it == _positions.end()) {
        _positions[id] = Position::byId(id);
    }
}

void Book::handleRemoved(const database::Value& value) {
    Id id = value.asString();
    auto it = _positions.find(id);
    if (it != _positions.end()) {
        Position& position = *it->second;
        position.remove();
        _positions.erase(it);
        delete &position;
    }
}

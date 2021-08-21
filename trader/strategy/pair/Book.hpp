#pragma once

#include <unordered_map>
#include "Defines.hpp"
#include "database/Array.hpp"
#include "exchanger/base/Identifier.hpp"

namespace database {
    class Value;
}

NS_BEGIN
class Position;

class Book : public database::Array, public Identifier {
public: // static
    typedef std::vector<Position*> Vector;
    typedef std::unordered_map<Id, Position*> Map;

    WRAPPER_CREATE(Book)
    WRAPPER_BYID(Book)

public: // methods
    virtual ~Book();

    bool add(Position* position);
    Vector positions() const;

protected: // methods
    Book(Id id);

    void handleAdded(const database::Value& value);
    void handleRemoved(const database::Value& value);

protected: // vars
    Map _positions;
};
NS_END

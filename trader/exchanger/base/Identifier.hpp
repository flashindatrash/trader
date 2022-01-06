#pragma once

#include <string>

class Identifier {
public: // static
    typedef std::string Id;

public: // methods
    void setId(Id id);
    const Id& id() const;

    bool empty() const;

protected: // methods
    Identifier() = default;

protected: // methods
    Id _identifier = "";
};

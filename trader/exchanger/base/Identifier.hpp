#pragma once

#include <string>

class Identifier {
public: // methods
    void setId(std::string id);
    const std::string& id() const;

protected: // methods
    Identifier() = default;

protected: // methods
    std::string _identifier = "";
};

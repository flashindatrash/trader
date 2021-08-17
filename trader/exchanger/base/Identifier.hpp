#pragma once

#include <string>

class Identifier {
public:
    void setId(std::string identifier);
    const std::string& id() const;

protected:
    std::string _identifier = "";
};

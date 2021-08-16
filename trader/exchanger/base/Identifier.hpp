#pragma once

#include <string>

class Identifier {
public:
    void setIdentifier(std::string identifier);
    const std::string& getIdentifier() const;

protected:
    std::string _identifier;
};

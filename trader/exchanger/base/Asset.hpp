#pragma once

#include "Identifier.hpp"
#include "BaseTypes.hpp"

class Asset : public Identifier {
public: // methods
    Asset() = default;
    Asset(const std::string& asset);

    const Quantity& getBalance() const;

    const char* c_str() const;
    operator std::string() const;

    const std::string operator+(const Asset& quote) const;
};

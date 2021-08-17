#include "Identifier.hpp"

void Identifier::setId(std::string identifier) {
    _identifier = identifier;
}

const std::string& Identifier::id() const {
    return _identifier;
}

#include "Identifier.hpp"

void Identifier::setIdentifier(std::string identifier) {
    _identifier = identifier;
}

const std::string& Identifier::getIdentifier() const {
    return _identifier;
}

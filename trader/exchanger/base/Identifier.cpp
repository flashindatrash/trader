#include "Identifier.hpp"

void Identifier::setId(std::string id) {
    _identifier = id;
}

const std::string& Identifier::id() const {
    return _identifier;
}

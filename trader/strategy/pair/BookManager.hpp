#pragma once

#include "Defines.hpp"

NS_BEGIN
class BookManager {
public: // static
    static BookManager* create();

public: // methods
    bool init();

protected: // methods
    BookManager() = default;
};
NS_END

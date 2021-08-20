#pragma once

#include "Defines.hpp"

NS_BEGIN
class Settings;
class Context;
class BookManager;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    bool init();
    void execute(const Context& context);

protected: // methods
    Algorithm() = default;

protected: // vars
    BookManager* _book = nullptr;
};
NS_END

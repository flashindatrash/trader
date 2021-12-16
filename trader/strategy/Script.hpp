//
// Created by Вадим Проскурин on 12.12.2021.
//

#pragma once

#include "Defines.hpp"

class lua_State;
struct OrderRequest;

NS_BEGIN
class Position;

class Script {
public: // static
    static Script* create(const std::string& file);

private: // static
    static int print(lua_State *L);
    static int balance(lua_State *L);
    static int dema(lua_State *L);

public: // methods
    bool open(OrderRequest& request);
    bool close(const Position& position);
    bool average(const Position& position);

private: // methods
    Script() = default;
    ~Script();

    bool init();
    bool load(const std::string& file);

    bool callPosition(const char* fn, const Position& position);

private: // vars
    lua_State* lua = nullptr;

};
NS_END

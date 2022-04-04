//
// Created by Вадим Проскурин on 12.12.2021.
//

#pragma once

#include <string>

class Position;
class lua_State;
class Settings;
struct OrderRequest;

namespace trader {

class Script {
public: // static
    static Script* create(const std::string& file);

private: // static
    static int bind_print(lua_State *L);
    static int bind_balance(lua_State *L);
    static int bind_price(lua_State *L);
    static int bind_topup(lua_State *L);
    static int bind_dema(lua_State *L);
    static int bind_chart(lua_State *L);

public: // methods
    bool main(const Settings& settings);
    bool open(OrderRequest& request);
    bool close(const Position& position);
    bool average(const Position& position);

private: // methods
    Script() = default;
    ~Script();

    bool init();
    bool load(const std::string& file);

    bool call_position(const char* fn, const Position& position);

private: // vars
    lua_State* lua = nullptr;

};
}

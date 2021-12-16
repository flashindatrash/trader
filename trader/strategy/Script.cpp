//
// Created by Вадим Проскурин on 12.12.2021.
//

#include <exchanger/indicator/DEMA.hpp>
#include "Script.hpp"
#include "Logger.hpp"
#include "Position.hpp"
#include "Context.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "util/StringUtil.hpp"
#include "lua.hpp"

NS_USE

static const char* METHOD_OPEN = "open";
static const char* METHOD_CLOSE = "close";
static const char* METHOD_AVERAGE = "average";

Script* Script::create(const std::string& file) {
    auto* script = new Script();
    if (not script->init() || not script->load(file)) {
        delete script;
        script = nullptr;
    }
    return script;
}

int Script::print(lua_State *L) {
    int args_size = lua_gettop(L);
    if (args_size < 1) {
        Logger::info("[script] failed to call `print`");
        return 0;
    }

    std::string value = lua_tostring(L, 1);
    Logger::info(util::format("[script] %s", value.c_str()));
    return 0;
}

int Script::balance(lua_State *L) {
    int args_size = lua_gettop(L);
    if (args_size < 1) {
        Logger::info("[script] failed to call `balance`");
        lua_pushinteger(L, 0);
        return 1;
    }

    std::string asset = lua_tostring(L, 1);
    lua_pushnumber(L, Asset(asset).balance());
    return 1;
}

int Script::dema(lua_State *L) {
    const Context* context = Context::current;
    if (context == nullptr || lua_gettop(L) < 2) {
        Logger::info("[script] failed to call `dema`");
        lua_pushinteger(L, OrderSide::Invalid);
        lua_pushinteger(L, OrderSide::Invalid);
        return 2;
    }

    size_t fast = lua_tointeger(L, 1);
    size_t slow = lua_tointeger(L, 2);

    DEMA indicator = DEMA(fast, slow);
    Context::current->load(indicator);

    lua_pushinteger(L, indicator.trend());
    lua_pushinteger(L, indicator.signal());
    return 2;
}

Script::~Script() {
    if (lua != nullptr)
        lua_close(lua);
}

bool Script::init() {
    lua = luaL_newstate();
    if (lua == nullptr) {
        Logger::info("luaL_loadfile isn't inited");
        return false;
    }

    luaL_openlibs(lua);

    lua_register(lua, "print", print);
    lua_register(lua, "balance", balance);
    lua_register(lua, "dema", dema);
    return true;
}

bool Script::load(const std::string& file) {
    if (luaL_loadfile(lua, file.c_str())) {
        Logger::info(util::format("luaL_loadfile failed: %s", lua_tostring(lua, -1)));
        lua_pop(lua, 1);
        return false;
    }

    if (lua_pcall(lua, 0, 0, 0)) {
        Logger::info(util::format("lua_pcall failed: %s", lua_tostring(lua, -1)));
        lua_pop(lua, 1);
        return false;
    }

    return  true;
}

bool Script::open(OrderRequest& request) {
    lua_getglobal(lua, METHOD_OPEN);
    if (not lua_isfunction(lua, -1))
        return false;

    lua_pcall(lua, 0, 2, 0);
    if (lua_isnil(lua, -1))
        return false;

    request.quantity *= lua_tonumber(lua, -1);
    lua_pop(lua,1);
    request.side = (OrderSide)lua_tointeger(lua, -1);
    lua_pop(lua,1);
    return true;
}

bool Script::close(const Position& position) {
    return callPosition(METHOD_CLOSE, position);
}

bool Script::average(const Position& position) {
    return callPosition(METHOD_AVERAGE, position);
}

bool Script::callPosition(const char* fn, const Position& position) {
    lua_getglobal(lua, fn);
    if (not lua_isfunction(lua, -1))
        return false;

    lua_newtable(lua);

    lua_pushstring(lua, position.symbol().baseAsset().c_str());
    lua_setfield(lua, -2, "baseAsset");

    lua_pushstring(lua, position.symbol().quoteAsset().c_str());
    lua_setfield(lua, -2, "quoteAsset");

    lua_pushinteger(lua, position.side());
    lua_setfield(lua, -2, "side");

    lua_pushnumber(lua, position.baseQuantity());
    lua_setfield(lua, -2, "baseQuantity");

    lua_pushnumber(lua, position.quoteQuantity());
    lua_setfield(lua, -2, "quoteQuantity");

    lua_pushnumber(lua, position.price());
    lua_setfield(lua, -2, "price");

    lua_pushnumber(lua, position.distance(Context::current->price(position.revert())));
    lua_setfield(lua, -2, "distance");

    lua_pushnumber(lua, position.change(Context::current->price(position.revert())));
    lua_setfield(lua, -2, "change");

    lua_pcall(lua, 1, 1, 0);
    if (lua_isnil(lua, -1))
        return false;

    bool result = lua_toboolean(lua, -1);
    lua_pop(lua,1);

    return result;
}
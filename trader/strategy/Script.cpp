//
// Created by Вадим Проскурин on 12.12.2021.
//

#include "Script.hpp"
#include "Logger.hpp"
#include "Position.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Time.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/indicator/DEMA.hpp"
#include "lua.hpp"

NS_USE

static const char* BIND_PRINT = "print";
static const char* BIND_BALANCE = "balance";
static const char* BIND_PRICE = "price";
static const char* BIND_TOPUP = "topup";
static const char* BIND_DEMA = "dema";
static const char* BIND_CHART = "chart";

static const char* METHOD_MAIN = "__main__";
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

int Script::bind_print(lua_State *L) {
    int args_size = lua_gettop(L);
    if (args_size < 1) {
        Logger::info("[script] failed to call `print`");
        return 0;
    }

    std::string value = lua_tostring(L, 1);
    Logger::info(util::format("%s%s%s", CYAN, value.c_str(), RESET));
    return 0;
}

int Script::bind_balance(lua_State *L) {
    int args_size = lua_gettop(L);
    if (args_size < 1) {
        Logger::info("[script] failed to call `balance`");
        lua_pushnumber(L, 0.0);
        return 1;
    }

    std::string asset = lua_tostring(L, 1);
    lua_pushnumber(L, Asset(asset).balance());
    return 1;
}

int Script::bind_price(lua_State *L) {
    const Context *context = Context::current;
    if (context == nullptr) {
        Logger::info("[script] failed to call `price`");
        lua_pushnumber(L, 0.0);
        return 1;
    }

    lua_pushnumber(L, context->price());
    return 1;
}

int Script::bind_topup(lua_State *L) {
    int args_size = lua_gettop(L);
    if (args_size < 2) {
        Logger::info("[script] failed to call `topup`");
        return 0;
    }

    std::string asset = lua_tostring(L, 1);
    Quantity quantity = lua_tonumber(L, 2);
    Exchanger().balance(Asset(asset))->gain(quantity);
    return 0;
}

int Script::bind_dema(lua_State *L) {
    const Context* context = Context::current;
    if (lua_gettop(L) < 2 || context == nullptr) {
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

int Script::bind_chart(lua_State *L) {
    int args_size = lua_gettop(L);
    if (args_size < 3) {
        Logger::info("[script] failed to call `chart`");
        lua_pushboolean(L, false);
        return 1;
    }

    std::string baseAsset = lua_tostring(L, 1);
    std::string quoteAsset = lua_tostring(L, 2);
    Symbol symbol(baseAsset, quoteAsset);

    ChartRequest request;
    request.interval = (ChartInterval)lua_tointeger(L, 3);

    int days = 1;
    if (args_size >= 4)
        days = (int)lua_tointeger(L, 4);

    time_t now = Time().ms();
    for (int i = days; i > 0; --i) {
        request.time_start = now - Timer::sDay * i;
        request.time_end = now - Timer::sDay * (i - 1);
        if (not Exchanger().loadCharts(symbol, request)) {
            lua_pushboolean(L, false);
            return 1;
        }
    }

    if (days <= 1) {
        Exchanger().listenCharts(symbol, request.interval);
        Exchanger().listenTickers(symbol);
    }

    lua_pushboolean(L, true);
    return 1;
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

    lua_register(lua, BIND_PRINT, bind_print);
    lua_register(lua, BIND_BALANCE, bind_balance);
    lua_register(lua, BIND_PRICE, bind_price);
    lua_register(lua, BIND_TOPUP, bind_topup);
    lua_register(lua, BIND_DEMA, bind_dema);
    lua_register(lua, BIND_CHART, bind_chart);
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

bool Script::main(const Settings& settings) {
    lua_getglobal(lua, METHOD_MAIN);
    if (not lua_isfunction(lua, -1))
        return false;

    lua_newtable(lua);

    lua_pushstring(lua, settings.symbol.baseAsset().c_str());
    lua_setfield(lua, -2, "baseAsset");

    lua_pushstring(lua, settings.symbol.quoteAsset().c_str());
    lua_setfield(lua, -2, "quoteAsset");

    lua_pushstring(lua, settings.mode.c_str());
    lua_setfield(lua, -2, "mode");

    if (lua_pcall(lua, 1, 1, 0)) {
        Logger::info(util::format("lua_pcall failed: %s", lua_tostring(lua, -1)));
        lua_pop(lua, 1);
        return false;
    }

    bool result = lua_toboolean(lua, -1);
    lua_pop(lua,1);

    return result;
}

bool Script::open(OrderRequest& request) {
    lua_getglobal(lua, METHOD_OPEN);
    if (not lua_isfunction(lua, -1))
        return false;

    if (lua_pcall(lua, 0, 2, 0)) {
        Logger::info(util::format("lua_pcall failed: %s", lua_tostring(lua, -1)));
        lua_pop(lua, 2);
        return false;
    }

    request.quantity = lua_tonumber(lua, -1);
    lua_pop(lua,1);
    request.side = (OrderSide)lua_tointeger(lua, -1);
    lua_pop(lua,1);
    return true;
}

bool Script::close(const Position& position) {
    return call_position(METHOD_CLOSE, position);
}

bool Script::average(const Position& position) {
    return call_position(METHOD_AVERAGE, position);
}

bool Script::call_position(const char* fn, const Position& position) {
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

    lua_pushnumber(lua, position.profit(Context::current->price(position.revert())));
    lua_setfield(lua, -2, "profit");

    if (lua_pcall(lua, 1, 1, 0))  {
        Logger::info(util::format("lua_pcall failed: %s", lua_tostring(lua, -1)));
        lua_pop(lua, 1);
        return false;
    }

    bool result = lua_toboolean(lua, -1);
    lua_pop(lua,1);

    return result;
}
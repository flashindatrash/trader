cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(traderbot C CXX)

# set build configuration
IF(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
ENDIF()

# override compiler flags
set(PROJ_WARNINGS "-Wno-write-strings -Wsign-compare -Wreturn-type -Wno-deprecated-declarations -Wno-deprecated-increment-bool")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -std=c++17 -g ${PROJ_WARNINGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -std=c++17 ${PROJ_WARNINGS}")

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
	list(APPEND ALL_LIBS ${OPENSSL_LIBRARIES})
	message("-- Found OpenSSL lib: ${OPENSSL_LIBRARIES}")
else()
	message(FATAL_ERROR "Could not find the OpenSSL library")
endif()

# curl
find_package(CURL REQUIRED)
if(CURL_FOUND)
	include_directories(${CURL_INCLUDE_DIR})
	list(APPEND ALL_LIBS ${CURL_LIBRARIES})
	message("-- Found CURL lib: ${CURL_LIBRARIES}")
else()
	message(FATAL_ERROR "Could not find the CURL library")
endif()

# jsoncpp
if(APPLE)
	set(JSONCPP_ROOT_DIR "/opt/homebrew/opt/jsoncpp")
	include_directories(${JSONCPP_ROOT_DIR}/include)
	list(APPEND ALL_LIBS ${JSONCPP_ROOT_DIR}/lib/libjsoncpp.dylib)
elseif(UNIX)
	include_directories(/usr/include/jsoncpp)
	list(APPEND ALL_LIBS /usr/lib/aarch64-linux-gnu/libjsoncpp.so)
endif()

# websockets
if(APPLE)
	set(WEBSOCKETS_ROOT_DIR "/opt/homebrew/opt/libwebsockets")
	include_directories(${WEBSOCKETS_ROOT_DIR}/include)
	list(APPEND ALL_LIBS ${WEBSOCKETS_ROOT_DIR}/lib/libwebsockets.dylib)
elseif(UNIX)
	include_directories(/usr/local/include/libwebsockets/)
	list(APPEND ALL_LIBS /usr/local/lib/libwebsockets.so)
endif()

# project sources
set(CORE_SRC
	core/core/App.cpp
	core/core/Config.cpp
	core/core/HttpApp.cpp
	core/core/Logger.cpp
	core/core/Time.cpp
	core/core/Version.cpp
	core/database/Database.cpp
	core/database/Object.cpp
	core/database/Value.cpp
	core/network/TCPClient.cpp
	core/network/TCPServer.cpp
	core/protocol/Position.cpp
	core/protocol/Stats.cpp
	core/protocol/Storage.cpp
	core/util/MathUtil.cpp
	core/util/StringUtil.cpp
)
set(TRADER_SRC
	trader/app/TraderApp.cpp
	trader/base/Settings.cpp
	trader/exchanger/Exchanger.cpp
	trader/exchanger/abstract/ExchangerController.cpp
	trader/exchanger/base/Asset.cpp
	trader/exchanger/base/Decimal.cpp
	trader/exchanger/base/Identifier.cpp
	trader/exchanger/base/OrderBase.cpp
	trader/exchanger/base/OrderCreator.cpp
	trader/exchanger/base/Position.cpp
	trader/exchanger/base/Report.cpp
	trader/exchanger/base/Storage.cpp
	trader/exchanger/base/Symbol.cpp
	trader/exchanger/base/Wallet.cpp
	trader/exchanger/gate/GateController.cpp
	trader/exchanger/gate/GateWebsocket.cpp
	trader/exchanger/indicator/DEMA.cpp
	trader/exchanger/indicator/EMA.cpp
	trader/exchanger/indicator/MACD.cpp
	trader/exchanger/wrapper/BalanceWrapper.cpp
	trader/exchanger/wrapper/BookWrapper.cpp
	trader/exchanger/wrapper/CandlestickPattern.cpp
	trader/exchanger/wrapper/CandlestickWrapper.cpp
	trader/exchanger/wrapper/ChartWrapper.cpp
	trader/exchanger/wrapper/OrderWrapper.cpp
	trader/exchanger/wrapper/PriceWrapper.cpp
	trader/main.cpp
	trader/processor/Algorithm.cpp
	trader/processor/Context.cpp
	trader/processor/Formatter.cpp
	trader/processor/Listener.cpp
	trader/processor/Runner.cpp
	trader/processor/Script.cpp
	trader/processor/Trader.cpp
)

# hiredis
set(HIREDIS_ROOT_DIR /usr/local/Cellar/hiredis/1.0.0)
find_library(HIREDIS_LIBRARY
	NAMES hiredis
	PATHS /usr/lib /usr/local/lib
)
include_directories("${HIREDIS_ROOT_DIR}/include")

# Lua 5.1
if(APPLE)
	set(LUA51_ROOT_DIR "/opt/local/lua51")
	find_path(LUA_INCLUDE_DIR
		NAMES lua.h
		PATHS "${LUA51_ROOT_DIR}/include"
		NO_DEFAULT_PATH
	)
	find_library(LUA_LIBRARIES
		NAMES lua
		PATHS "${LUA51_ROOT_DIR}/lib"
		NO_DEFAULT_PATH
	)
else()
	find_path(LUA_INCLUDE_DIR
		NAMES lua.h
		PATHS /usr/include/lua5.1 /usr/local/include/lua5.1
	)
	find_library(LUA_LIBRARIES NAMES lua5.1 lua)
endif()

if(NOT LUA_INCLUDE_DIR OR NOT LUA_LIBRARIES)
	message(FATAL_ERROR "Could not find Lua 5.1")
endif()

message(STATUS "Found Lua51 include: ${LUA_INCLUDE_DIR}")
message(STATUS "Found Lua51 library: ${LUA_LIBRARIES}")

include_directories(
	${PROJECT_SOURCE_DIR}/core
	${PROJECT_SOURCE_DIR}/trader
	${LUA_INCLUDE_DIR}
)
list(APPEND ALL_LIBS ${HIREDIS_LIBRARY} ${LUA_LIBRARIES})

# add for stacktrace
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX AND NOT APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -pthread")
endif()

add_library(core ${CORE_SRC})
add_executable(trader ${TRADER_SRC})
target_link_libraries(trader core ${ALL_LIBS})

install(
	TARGETS trader
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

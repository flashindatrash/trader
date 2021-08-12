cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(trader-bot C CXX)

# set build configuration
IF(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
ENDIF()

# override compiler flags
set(PROJ_WARNINGS "-Wno-write-strings -Wsign-compare -Wreturn-type -Wno-deprecated-declarations -Wno-deprecated-increment-bool")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -std=c++11 -g ${PROJ_WARNINGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -std=c++11 ${PROJ_WARNINGS}")

# openssl
if(APPLE)
        SET(OPENSSL_ROOT_DIR /usr/local/opt/openssl@1.1)
endif()

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

# hiredis
SET(HIREDIS_ROOT_DIR /usr/local/Cellar/hiredis/1.0.0)
FIND_LIBRARY(HIREDIS_LIBRARY
        NAMES hiredis
        PATHS /usr/lib /usr/local/lib
)
include_directories("${HIREDIS_ROOT_DIR}/include")
list(APPEND ALL_LIBS ${HIREDIS_LIBRARY})
message("-- Found hiredis lib: ${HIREDIS_LIBRARY}")

# jsoncpp
set(JSONCPP_ROOT_DIR "/usr/local/Cellar/jsoncpp/1.9.4_1")
FIND_LIBRARY(JSONCPP_LIBRARY
        NAMES libjsoncpp
        PATHS /usr/lib /usr/local/lib
)
include_directories(${JSONCPP_ROOT_DIR}/include)
list(APPEND ALL_LIBS ${JSONCPP_LIBRARY})
message("-- Found jsoncpp lib: ${JSONCPP_LIBRARY}")

# websockets
set(WEBSOCKETS_ROOT_DIR "/usr/local/Cellar/libwebsockets/4.2.1")
FIND_LIBRARY(WEBSOCKETS_LIBRARY
        NAMES libwebsockets
        PATHS /usr/lib /usr/local/lib
)
include_directories(${WEBSOCKETS_ROOT_DIR}/include)
list(APPEND ALL_LIBS ${WEBSOCKETS_ROOT_DIR}/lib/libwebsockets.dylib)
message("-- Found websockets lib: ${WEBSOCKETS_LIBRARY}")

# core
include_directories(${PROJECT_SOURCE_DIR}/core)

subdirs(
	core
	tools
	trader
)

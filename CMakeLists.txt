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

find_package(PkgConfig)

# openssl
if(APPLE)
        SET(OPENSSL_ROOT_DIR /usr/local/opt/openssl@1.1)
endif()

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    list(APPEND ALL_LIBS ${OPENSSL_LIBRARIES})
    message("-- OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
else()
    message(FATAL_ERROR "Could not find the OpenSSL library")
endif()

#set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
#include_directories(${OPENSSL_ROOT_DIR}/include)
#list(APPEND ALL_LIBS ${OPENSSL_ROOT_DIR}/lib/libssl.dylib)
#list(APPEND ALL_LIBS ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib)

# jsoncpp
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
list(APPEND ALL_LIBS ${JSONCPP_LIBRARIES})
include_directories(${JSONCPP_INCLUDE_DIRS})

#set(JSONCPP_ROOT_DIR "/usr/local/Cellar/jsoncpp/1.9.4_1")
#include_directories(${JSONCPP_ROOT_DIR}/include)
#list(APPEND ALL_LIBS ${JSONCPP_ROOT_DIR}/lib/libjsoncpp.dylib)

# websockets
pkg_check_modules(LIB_WEBSOCKETS REQUIRED libwebsockets)
message("-- Found libwebsockets library: ${LIB_WEBSOCKETS_LIBRARIES}")
list(APPEND ALL_LIBS ${LIB_WEBSOCKETS_LIBRARIES})
include_directories(${LIB_WEBSOCKETS_INCLUDE_DIRS})

#set(WEBSOCKETS_ROOT_DIR "/usr/local/Cellar/libwebsockets/4.2.1")
#include_directories(${WEBSOCKETS_ROOT_DIR}/include)
#list(APPEND ALL_LIBS ${WEBSOCKETS_ROOT_DIR}/lib/libwebsockets.dylib)

# curl
find_package(CURL REQUIRED)
if(CURL_FOUND)
    message("-- Found CURL headers: ${CURL_INCLUDE_DIR}")
    include_directories(${CURL_INCLUDE_DIR})

    find_library(CURL_LIB
            NAMES curl
            PATHS /usr/lib /usr/local/lib
    )

    if(CURL_LIB)
            list(APPEND ALL_LIBS ${CURL_LIB})
            message("-- Found CURL library: ${CURL_LIB}")
    else()
            message(FATAL_ERROR "Cannot find CURL library")
    endif()
else()
    message(FATAL_ERROR "Could not find the CURL library")
endif()

# hiredis
FIND_LIBRARY(HIREDIS_LIBRARY
        NAMES hiredis
        PATHS /usr/lib /usr/local/lib
)
list(APPEND ALL_LIBS ${HIREDIS_LIBRARY})

#set(CURL_ROOT_DIR "/usr/local/Cellar/curl/7.78.0")
#include_directories(${CURL_ROOT_DIR}/include)
#list(APPEND ALL_LIBS ${CURL_ROOT_DIR}/lib/libcurl.dylib)

# core
include_directories(/usr/local/include)
include_directories(${PROJECT_SOURCE_DIR}/core)

subdirs(
	core
	tools
	trader
)

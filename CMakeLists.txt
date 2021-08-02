cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(trader-bot C CXX)

# override compiler flags
set(PROJ_WARNINGS "-Wno-write-strings -Wsign-compare -Wreturn-type -Wno-deprecated-declarations -Wno-deprecated-increment-bool")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -std=c++11 -g ${PROJ_WARNINGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -std=c++11 ${PROJ_WARNINGS}")

# openssl
set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
include_directories(${OPENSSL_ROOT_DIR}/include)
list(APPEND ALL_LIBS ${OPENSSL_ROOT_DIR}/lib/libssl.dylib)
list(APPEND ALL_LIBS ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib)

# jsoncpp
set(JSONCPP_ROOT_DIR "/usr/local/Cellar/jsoncpp/1.9.4_1")
include_directories(${JSONCPP_ROOT_DIR}/include)
list(APPEND ALL_LIBS ${JSONCPP_ROOT_DIR}/lib/libjsoncpp.dylib)

# websockets
set(WEBSOCKETS_ROOT_DIR "/usr/local/Cellar/libwebsockets/4.2.1")
include_directories(${WEBSOCKETS_ROOT_DIR}/include)
list(APPEND ALL_LIBS ${WEBSOCKETS_ROOT_DIR}/lib/libwebsockets.dylib)

# curl
set(CURL_ROOT_DIR "/usr/local/Cellar/curl/7.78.0")
include_directories(${CURL_ROOT_DIR}/include)
list(APPEND ALL_LIBS ${CURL_ROOT_DIR}/lib/libcurl.dylib)

# core
include_directories(${PROJECT_SOURCE_DIR}/core)

subdirs(
	core
	trader
)

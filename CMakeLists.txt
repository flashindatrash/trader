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
	list(APPEND ALL_LIBS /usr/lib/x86_64-linux-gnu/libjsoncpp.so)
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

subdirs(
	core
	binacpp
	trader
)

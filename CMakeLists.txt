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

subdirs(
	core
	tools
	trader
	telegram
)

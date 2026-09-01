set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE TRADER_SRC *.hpp *.cpp)

# add for stacktrace
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX AND NOT APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -pthread")
endif()

# libraries
list(APPEND ALL_LIBS ${HIREDIS_LIBRARY})

# lua 5.1
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

if(NOT LUA_INCLUDE_DIR OR NOT LUA_LIBRARIES)
	message(FATAL_ERROR "Could not find Lua 5.1 in ${LUA51_ROOT_DIR}")
endif()

message("-- Found Lua51 include: ${LUA_INCLUDE_DIR}")
message("-- Found Lua51 library: ${LUA_LIBRARIES}")

include_directories(${LUA_INCLUDE_DIR})
list(APPEND ALL_LIBS ${LUA_LIBRARIES})

# includes
include_directories(${PROJECT_SOURCE_DIR}/core)

add_executable(trader ${TRADER_SRC})
target_link_libraries(trader core ${ALL_LIBS})

install(
	TARGETS trader
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

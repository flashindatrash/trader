set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE TRADER_SRC *.hpp *.cpp)

# add for stacktrace
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX AND NOT APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -pthread")
endif()

# hiredis
list(APPEND ALL_LIBS ${HIREDIS_LIBRARY})
message("-- Found hiredis lib: ${HIREDIS_LIBRARY}")

include_directories(${PROJECT_SOURCE_DIR}/binacpp/src/)
include_directories(${PROJECT_SOURCE_DIR}/core)

add_executable(trader ${TRADER_SRC})
target_link_libraries(trader core binance ${ALL_LIBS})

install(
	TARGETS trader
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

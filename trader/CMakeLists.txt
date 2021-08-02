set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE TRADER_SRC *.hpp *.cpp)
file(GLOB BINACPP_SRC ../binacpp/src/*)

include_directories(../binacpp/src/)

add_executable(trader ${BINACPP_SRC} ${TRADER_SRC})
target_link_libraries(trader core ${ALL_LIBS})

install(
	TARGETS trader
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE TOOLS_SRC *.hpp *.cpp)
file(GLOB BINACPP_SRC ../binacpp/src/*)

include_directories(../binacpp/src/)

add_executable(tools ${BINACPP_SRC} ${TOOLS_SRC})
target_link_libraries(tools core ${ALL_LIBS})

install(
	TARGETS tools
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

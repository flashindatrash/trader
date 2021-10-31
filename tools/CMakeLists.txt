set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE TOOLS_SRC *.hpp *.cpp)

include_directories(${PROJECT_SOURCE_DIR}/binacpp/src/)
include_directories(${PROJECT_SOURCE_DIR}/core)

add_executable(tools ${TOOLS_SRC})
target_link_libraries(tools core binance ${ALL_LIBS})

install(
	TARGETS tools
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

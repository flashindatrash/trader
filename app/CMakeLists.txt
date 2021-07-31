set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE APP_SRC *.hpp *.cpp)
file(GLOB BINACPP_SRC ../binacpp/src/*)

include_directories(../binacpp/src/)

add_executable(app ${BINACPP_SRC} ${APP_SRC})
target_link_libraries(app Miscellaneous ${ALL_LIBS})

install(
	TARGETS app
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

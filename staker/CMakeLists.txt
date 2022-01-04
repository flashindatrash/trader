set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE STAKER_SRC *.hpp *.cpp)

# includes
include_directories(${PROJECT_SOURCE_DIR}/binacpp/src/)
include_directories(${PROJECT_SOURCE_DIR}/core)
include_directories(${PROJECT_SOURCE_DIR}/protocol)

add_executable(staker ${STAKER_SRC})
target_link_libraries(staker core protocol binance ${ALL_LIBS})

install(
        TARGETS staker
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

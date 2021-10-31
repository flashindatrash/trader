set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB CORE_SRC *.hpp *.cpp)

add_library(core ${CORE_SRC})
target_link_libraries(core)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB CORE_SOURCES *.hpp *.cpp)

add_library(core ${CORE_SOURCES})
target_link_libraries(core)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE PROTOCOL_SRC *.hpp *.cpp)

# core
include_directories(${PROJECT_SOURCE_DIR}/core)

add_library(protocol ${PROTOCOL_SRC})
target_link_libraries(protocol core)
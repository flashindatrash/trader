set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB MISC_SOURCES *.hpp *.cpp)

add_library(Miscellaneous ${MISC_SOURCES})
target_link_libraries(Miscellaneous)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB_RECURSE CORE_SRC *.hpp *.cpp)

# hiredis
SET(HIREDIS_ROOT_DIR /usr/local/Cellar/hiredis/1.0.0)
FIND_LIBRARY(HIREDIS_LIBRARY
        NAMES hiredis
        PATHS /usr/lib /usr/local/lib
        )
include_directories("${HIREDIS_ROOT_DIR}/include")

add_library(core ${CORE_SRC})
target_link_libraries(core)

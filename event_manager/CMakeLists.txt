set(Boost_USE_MULTITHREADED ON)
file(GLOB_RECURSE EVENT_MANAGER_SRC *.hpp *.cpp)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

# libraries
list(APPEND ALL_LIBS /usr/local/lib/libTgBot.a)
list(APPEND ALL_LIBS ${CMAKE_THREAD_LIBS_INIT})
list(APPEND ALL_LIBS ${OPENSSL_LIBRARIES})
list(APPEND ALL_LIBS ${Boost_LIBRARIES})
list(APPEND ALL_LIBS ${CURL_LIBRARIES})
list(APPEND ALL_LIBS ${HIREDIS_LIBRARY})

# includes
include_directories(${PROJECT_SOURCE_DIR}/core)
include_directories(${PROJECT_SOURCE_DIR}/protocol)

add_executable(event_manager ${EVENT_MANAGER_SRC})
target_link_libraries(event_manager protocol ${ALL_LIBS})

install(
        TARGETS event_manager
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
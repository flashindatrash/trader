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

# hiredis
list(APPEND ALL_LIBS ${HIREDIS_LIBRARY})

# core
include_directories(${PROJECT_SOURCE_DIR}/core)

add_executable(event_manager ${EVENT_MANAGER_SRC})
target_link_libraries(event_manager core /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${ALL_LIBS})

install(
        TARGETS event_manager
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
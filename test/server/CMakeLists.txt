
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_server testServer.cpp)
add_definitions(-g -ggdb3 -Wall -Wextra -O0)
target_link_libraries(test_server boost_system)

include_directories(
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/src/include
)


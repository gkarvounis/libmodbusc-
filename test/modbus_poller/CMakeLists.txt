
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(testmodbuspoller test_modbuspoller.cpp)
add_definitions(-Wall -Wextra -g -ggdb3 -O0)
target_link_libraries(testmodbuspoller boost_system boost_program_options pthread)

include_directories(
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/src/include
)

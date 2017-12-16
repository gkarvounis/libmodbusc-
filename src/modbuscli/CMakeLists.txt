
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(modbuscli main.cpp)
add_definitions(-g -ggdb3 -Wall -Wextra -O0)
target_link_libraries(modbuscli boost_system boost_program_options)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/include
)


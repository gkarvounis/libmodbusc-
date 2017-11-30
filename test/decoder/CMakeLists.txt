
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(testdecoder testDecoder.cpp)

include_directories(
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/src/include
)

cmake_minimum_required(VERSION 3.17)
project(Compress)

set(CMAKE_CXX_STANDARD 20)
set (SOURCE_FILES utils.cpp utils.h rle.cpp rle.h huffman.cpp huffman.h)

add_executable(${PROJECT_NAME} main.cpp ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})

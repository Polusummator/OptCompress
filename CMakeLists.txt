cmake_minimum_required(VERSION 3.17)
project(Compress)

set(CMAKE_CXX_STANDARD 20)
set (SOURCE_FILES src/utils.cpp src/utils.h src/rle.cpp src/rle.h src/huffman.cpp src/huffman.h src/lzw.cpp src/lzw.h)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})

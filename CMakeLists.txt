cmake_minimum_required(VERSION 3.10)

project(mymuduo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

include_directories(${PROJECT_SOURCE_DIR}/MYMUDUO)

file(GLOB MYMUDUO_SRC ${PROJECT_SOURCE_DIR}/MYMUDUO/*.cc)

add_executable(muduo_server muduo_server.cpp ${MYMUDUO_SRC})

target_link_libraries(muduo_server pthread)
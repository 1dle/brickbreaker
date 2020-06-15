cmake_minimum_required(VERSION 3.10.2)

set (CMAKE_CXX_STANDARD 11)

project(BrickBreaker)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(BrickBreaker src/main.cpp)
target_link_libraries(BrickBreaker ${CURSES_LIBRARIES})
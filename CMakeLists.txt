cmake_minimum_required(VERSION 3.3)

project(terminal-motor-cntrl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES main.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})

add_executable(terminal-motor-cntrl ${SOURCE_FILES})

target_link_libraries(terminal-motor-cntrl ${CURSES_LIBRARIES})
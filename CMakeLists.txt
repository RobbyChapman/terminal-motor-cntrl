cmake_minimum_required(VERSION 3.3)

project(terminal-motor-cntrl)


set(SOURCE_FILES main.c TerminalMotorController.h TerminalMotorController.c)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})

add_executable(terminal-motor-cntrl ${SOURCE_FILES})

target_link_libraries(terminal-motor-cntrl ${CURSES_LIBRARIES})
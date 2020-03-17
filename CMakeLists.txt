cmake_minimum_required(VERSION 3.15)
project(CSGO_External)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(CSGO_External main.cpp HazeDumper.cpp HazeDumper.h ProcessManager.cpp ProcessManager.h Cheats.cpp Cheats.h)
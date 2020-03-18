#Find Sources
file(GLOB SRCS ./**.cpp ./**.c)

#Haze Dump Importer Library
add_library(HazeImporter ${SRCS})

#Define include folder
target_include_directories(HazeImporter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
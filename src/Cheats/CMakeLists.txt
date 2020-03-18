#Find Sources
file(GLOB SRCS ./**.cpp ./**.c)

#Cheats Library
add_library(Cheats ${SRCS})

#Define include folder
target_include_directories(Cheats PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
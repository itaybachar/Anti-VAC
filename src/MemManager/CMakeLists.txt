#Find Sources
file(GLOB SRCS ./**.cpp ./**.c)

#Memory Manager Library
add_library(MemMan ${SRCS})

#Define include folder
target_include_directories(MemMan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
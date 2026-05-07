cmake_minimum_required(VERSION 3.25)
project(dectalkmini)

file(GLOB_RECURSE SOURCES src/**.c)

add_library(ppr STATIC ${SOURCES})
target_include_directories(ppr PUBLIC include)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(ppr PUBLIC ws2_32)
else()
	target_link_libraries(ppr PUBLIC pthread)
endif()

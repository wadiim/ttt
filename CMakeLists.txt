cmake_minimum_required(VERSION 3.0)

project(ttt C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Suppress Visual Studio deprecation warnings.
if (MSVC)
	add_definitions(-D _CRT_SECURE_NO_WARNINGS)
endif(MSVC)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)

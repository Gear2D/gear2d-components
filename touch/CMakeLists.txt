project(touch)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" "${CMAKE_MODULE_PATH}")

find_package(Gear2D REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${Gear2D_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})

set(CMAKE_BUILD_TYPE "Debug")

add_definitions(-std=c++11)

file(GLOB touch_sources *.cc *.h)

add_definitions(-DLOGTRACE)

g2dcomponent(FAMILY touch TYPE touch SOURCES ${touch_sources})
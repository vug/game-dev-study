cmake_minimum_required(VERSION 3.20)

project(GameDevStudy
  VERSION 0.1
  DESCRIPTION "Study making some classic, simple games")

include(dependencies/SDL2-2.24.0/cmake/sdl2-config.cmake)
include(dependencies/SDL2-2.24.0/cmake/sdl2-config-version.cmake)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(games)


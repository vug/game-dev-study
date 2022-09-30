cmake_minimum_required(VERSION 3.20)

project(GameDevStudy
  VERSION 0.1
  DESCRIPTION "Study making some classic, simple games")

include(dependencies/SDL2-2.24.0/cmake/sdl2-config.cmake)
include(dependencies/SDL2-2.24.0/cmake/sdl2-config-version.cmake)

include(dependencies/SDL2_ttf-2.20.1/cmake/sdl2_ttf-config.cmake)
include(dependencies/SDL2_ttf-2.20.1/cmake/sdl2_ttf-config-version.cmake)

add_subdirectory(games/lib)
add_subdirectory(games/game1)

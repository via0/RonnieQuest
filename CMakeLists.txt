cmake_minimum_required(VERSION 3.10)
project(ronnie C)

# set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# find SDL2 et al
find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SDL2_TTF REQUIRED SDL2_ttf)

# include directories
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

# add executable
add_executable(ronnie
  src/main.c
  src/game.c
  src/renderer.c
)

# Link SDL2
target_link_libraries(ronnie ${SDL2_LIBRARIES} SDL2_ttf m)

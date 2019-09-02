cmake_minimum_required(VERSION 3.0)
project(omni)

file(GLOB src "*.cpp")

add_executable(chs ${src})

set(CMAKE_CXX_STANDARD 17)

set(SDL "-lSDL2 -lSDL2_ttf")
set(OGL "-lGLEW -lGL -lGLU")

target_link_libraries(chs ${SDL} ${OGL})

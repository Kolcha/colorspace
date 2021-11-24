cmake_minimum_required(VERSION 3.14)

project(colorspace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(colorspace
    main.cpp
    polygon.cpp
    polygon.hpp
)

install(TARGETS colorspace)

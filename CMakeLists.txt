cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(box-labeler)

find_package(Qt5PrintSupport 5.7 REQUIRED)
find_package(Qt5Widgets 5.7 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)

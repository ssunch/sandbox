cmake_minimum_required(VERSION 3.5)

project(bmpViewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH /home/sean/work/Qt/5.13.1/gcc_64) #
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(bmpViewer
  main.cpp
  imageviewer.cpp
  imageviewer.h
  viewer.ui
)

target_link_libraries(bmpViewer PRIVATE Qt5::Widgets)

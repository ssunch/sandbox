cmake_minimum_required(VERSION 3.16)

project(imageViewer
    VERSION 1.0
    LANGUAGES C
)

find_package(X11 REQUIRED)
find_package(JPEG REQUIRED)

add_executable(imgViewer
    viewer.c
    bmpprocess.c
    jpgprocess.c
    util.c
)

target_include_directories(imgViewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(imgViewer PRIVATE
    X11::X11
    JPEG::JPEG
)

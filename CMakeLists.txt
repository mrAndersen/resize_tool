cmake_minimum_required(VERSION 3.12)
project(resize_tool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

# need global libs
# sudo apt install libvips-dev libglib

find_package(PkgConfig REQUIRED)
pkg_check_modules(VIPS REQUIRED vips vips-cpp gobject-2.0)

message(${VIPS_LIBRARIES})
message(${VIPS_INCLUDE_DIRS})

add_executable(
        resize_tool
        main.cpp
)

target_link_libraries(
        resize_tool PUBLIC
        ${VIPS_LIBRARIES}
)

target_include_directories(
        resize_tool PUBLIC
        ${VIPS_INCLUDE_DIRS}
)
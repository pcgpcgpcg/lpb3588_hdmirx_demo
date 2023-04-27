cmake_minimum_required(VERSION 3.0)
project(v4l2_plane)

# find_package(Libv4l2 REQUIRED)

set(SOURCE src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
# find_package(Libv4l2 REQUIRED)
# add_library(linux-v4l2 MODULE)
# target_link_libraries(-llibv4l)
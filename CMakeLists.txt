cmake_minimum_required(VERSION 3.0)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

project(v4l2_plane)

# find_package(Libv4l2 REQUIRED)

set(SOURCE src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
# find_package(Libv4l2 REQUIRED)
# add_library(linux-v4l2 MODULE)
# target_link_libraries(-llibv4l)
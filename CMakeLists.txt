cmake_minimum_required(VERSION 3.15)
project(my_project LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)

# Let Conan set up toolchain and dependency paths
find_package(fmt REQUIRED)

add_executable(my_project src/main.cpp)
target_link_libraries(my_project PRIVATE fmt::fmt)


cmake_minimum_required(VERSION 3.10)
project(yttria VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

file(GLOB_RECURSE YTTRIA_SRC CONFIGURE_DEPENDS src/*.cpp)

add_library(yttria STATIC ${YTTRIA_SRC})

target_include_directories(yttria
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${GLFW_INCLUDE_DIRS}
		${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(yttria
	PUBLIC
		Vulkan::Vulkan
		${GLFW_LIBRARIES}
)

add_subdirectory(examples/dev_app)


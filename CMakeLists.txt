cmake_minimum_required(VERSION 3.24)
project(CraftEngine)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

INCLUDE_DIRECTORIES(src/glm)
LINK_DIRECTORIES(src/glm)


add_executable(CraftEngine main.cpp src/App.cpp src/App.h src/utils.hpp)

target_link_libraries(CraftEngine Vulkan::Vulkan)
target_link_libraries(CraftEngine glfw)



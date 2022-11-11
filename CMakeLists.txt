cmake_minimum_required(VERSION 3.24)
project(CraftEngine)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

INCLUDE_DIRECTORIES(src/glm)
LINK_DIRECTORIES(src/glm)


add_executable(CraftEngine main.cpp src/App.cpp src/App.h src/utils.hpp src/vk_instance.cpp src/vk_instance.h src/vk_graphic_device.cpp src/vk_graphic_device.h src/vk_graphic_device.cpp src/vk_window.cpp src/vk_window.h src/shaders/compile_Spir_V.h src/shaders/compile_Spir_V.cpp src/vk_renderer.cpp src/vk_renderer.h src/debug/log.h src/debug/time.h src/debug/time.cpp src/debug/time.h)

target_link_libraries(CraftEngine Vulkan::Vulkan)
target_link_libraries(CraftEngine glfw)



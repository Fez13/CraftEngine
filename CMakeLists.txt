cmake_minimum_required(VERSION 3.22.1)
project(CraftEngine)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

INCLUDE_DIRECTORIES(src/vendor/glm)
LINK_DIRECTORIES(src/vendor/glm)


add_executable(CraftEngine main.cpp src/core/App.cpp src/core/App.h src/utils/utils.hpp src/core/vk_instance.cpp src/core/vk_instance.h src/gpu/vk_graphic_device.cpp src/gpu/vk_graphic_device.h src/rendered/vk_window.cpp src/rendered/vk_window.h src/shaders/compile_Spir_V.h src/shaders/compile_Spir_V.cpp src/rendered/vk_renderer.cpp src/rendered/vk_renderer.h src/debug/log.h src/debug/time.h src/debug/time.cpp src/gpu/vk_device_abstraction.cpp src/gpu/vk_device_abstraction.h src/gpu/vk_shader.cpp src/gpu/vk_shader.h src/gpu/vk_buffer.cpp src/gpu/vk_buffer.h src/rendered/mesh.h src/rendered/mesh.cpp src/rendered/vertex.cpp src/rendered/vertex.h src/rendered/vao.cpp src/rendered/vao.h src/rendered/ebo.cpp src/rendered/ebo.h src/utils/frameRate.cpp src/utils/frameRate.h src/rendered/camera.cpp src/rendered/camera.h src/input/input.cpp src/input/input.h src/gameObjects/entity.cpp src/gameObjects/entity.h src/gameObjects/entitiesExtencions.cpp src/gameObjects/entitiesExtencions.h src/input/keyCodes.h src/gameObjects/extencionsLabels.h  src/gpu/texture.h src/gpu/texture.cpp src/rendered/imageUtils.hpp src/vendor/stb/stb_image.h src/vendor/stb/stb.cpp src/vendor/tinyobjloader/tiny_obj_loader.h src/vendor/tinyobjloader/tiny_obj_loader.cpp)

target_link_libraries(CraftEngine Vulkan::Vulkan)
target_link_libraries(CraftEngine glfw)



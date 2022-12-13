#version 450

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec4 color_in;

layout(location = 0) out vec4 fragColor_out;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 cameraMatrix;
} ubo;

layout(push_constant) uniform push_data{
    mat4 transform;
}push;

void main() {
    gl_Position = ubo.cameraMatrix *(push.transform * vec4(position_in,1.0f));

    fragColor_out = color_in;
}
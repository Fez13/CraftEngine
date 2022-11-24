#version 450

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec4 color_in;

layout(location = 0) out vec4 fragColor_out;

void main() {
    gl_Position = vec4(position_in, 1.0);
    fragColor_out = color_in; // Should be rgba
}
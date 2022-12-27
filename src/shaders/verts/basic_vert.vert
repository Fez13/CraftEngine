#version 450

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec4 color_in;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor_out;
layout(location = 1) out vec2 fragTexCoord;


layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 projection;
    mat4 view;
} ubo;

layout(push_constant) uniform push_data{
    mat4 transform;
    bool hasTexture;
}push;

const vec3 lightDirection = normalize(vec3(-1,-3,-1));


void main() {
    gl_Position = (ubo.projection * ubo.view) * (push.transform * vec4(position_in,1.0f));

    fragColor_out = color_in;
    fragTexCoord = inTexCoord; 
}
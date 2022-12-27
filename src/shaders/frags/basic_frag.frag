#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform push_data{
    mat4 transform;
    bool hasTexture;
}push;


void main() {
    if(push.hasTexture){
        outColor = fragColor * texture(texSampler,fragTexCoord);
    }else{
        outColor = fragColor;
    }
}

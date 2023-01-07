#version 430 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexture;

#include "_uniforms.glsl"

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexture;

void main()
{
    gl_Position = projection * view * model * vec4(vPosition, 1);

    fPosition = vPosition;
    fNormal = vNormal;
    fTexture = vTexture;
}
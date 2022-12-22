#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vTexture;
layout(location = 2) in vec3 vNormal;

#include "_uniforms.glsl"

out vec3 fPosition;
out vec3 fTexture;
out vec3 fNormal;

void main() {

    gl_Position = projection * view * model * vec4(vPosition, 1);

    fPosition = vPosition;
    fTexture = vTexture;
    fNormal = vNormal;
}
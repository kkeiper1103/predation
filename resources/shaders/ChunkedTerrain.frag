#version 430 core

#include "_uniforms.glsl"

in vec3 fPosition;
in vec3 fTexture;
in vec3 fNormal;

out vec4 FragColor;

void main() {
    FragColor = vec4(1, 1, 1, 1);
}
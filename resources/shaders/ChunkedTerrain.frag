#version 430 core

#include "_uniforms.glsl"

in vec3 fPosition;
in vec3 fTexture;
in vec3 fNormal;

uniform sampler2DArray terrain;

out vec4 FragColor;

void main() {
    FragColor = texture(terrain, fTexture);
}
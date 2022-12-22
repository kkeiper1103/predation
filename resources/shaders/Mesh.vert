#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTextureCoords;

#include "_uniforms.glsl"

out vec2 fTextureCoords;

void main() {

    gl_Position = projection * view * model * vec4(vPosition, 1);

    fTextureCoords = vTextureCoords;
}
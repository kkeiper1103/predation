#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vTextureCoords;
layout(location = 3) in vec2 vTextureCoordsB;
layout(location = 4) in vec3 vNormal;

#include "_uniforms.glsl"

out vec3 fNormal;
out vec2 fTextureCoords;

void main() {

    gl_Position = projection * view * model * vec4(vPosition, 1);

    fNormal = vNormal;
    fTextureCoords = vTextureCoords;
}
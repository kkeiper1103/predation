#version 430 core

layout (location = 0) in vec3 aPos;

#include "_uniforms.glsl"

out vec3 TexCoords;


void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
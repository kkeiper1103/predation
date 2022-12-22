#version 430 core

in vec2 TexCoords; // direction vector representing a 3D texture coordinate

#include "_uniforms.glsl"

uniform float offset;
uniform sampler2D skybox; // cubemap texture sampler
uniform sampler2D normalMap;

out vec4 FragColor;

void main()
{
    FragColor = texture(skybox, vec2(TexCoords) + vec2(offset, offset / 2));
}
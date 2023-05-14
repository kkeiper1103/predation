#version 430 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexture;

#include "_uniforms.glsl"

uniform float offset;
uniform sampler2D skybox; // cubemap texture sampler

out vec4 FragColor;

void main()
{
    // animate the texture in the x direction
    FragColor = texture(skybox, fTexture);
    // FragColor = vec4(1, .4, .4, 1);
}
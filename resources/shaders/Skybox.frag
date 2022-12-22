#version 430 core

in vec3 TexCoords; // direction vector representing a 3D texture coordinate

#include "_uniforms.glsl"

uniform float offset;
uniform samplerCube skybox; // cubemap texture sampler

out vec4 FragColor;

void main()
{
    // animate the texture in the x direction
    FragColor = texture(skybox, vec3(TexCoords) + vec3(-offset, 0, 0));
}
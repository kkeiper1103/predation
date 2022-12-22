#version 430 core

in vec3 textureDir; // direction vector representing a 3D texture coordinate

#include "_uniforms.glsl"
uniform samplerCube skybox; // cubemap texture sampler

out vec4 FragColor;

void main()
{
    FragColor = texture(skybox, textureDir);
}
#version 430 core

in vec2 fTextureCoords;

#include "_uniforms.glsl"

uniform sampler2D diffuse;
uniform sampler2D specular;

out vec4 FragColor;

void main() {
    FragColor = texture(diffuse, fTextureCoords);

    if(FragColor.a < 0.05) discard;
}
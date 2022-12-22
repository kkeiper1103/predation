#version 430 core

in vec3 fNormal;
in vec2 fTextureCoords;

#include "_uniforms.glsl"

uniform sampler2D skin;

out vec4 FragColor;

void main() {
    FragColor = vec4(1, 1, 1, 1);

    // FragColor = texture(skin, fTextureCoords);
}
#version 430 core


in vec3 fPosition;
in float visibility;

#include "_uniforms.glsl"

uniform vec3 lineColor;
uniform sampler2D diffuse;
uniform sampler2D specular;

out vec4 FragColor;

void main() {
    FragColor = vec4(lineColor, 1);
}
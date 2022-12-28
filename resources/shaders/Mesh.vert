#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTextureCoords;

#include "_uniforms.glsl"

out vec2 fTextureCoords;
out float visibility;

const float density = 0.0135;
const float gradient = 5.0;

void main() {
    gl_Position = projection * view * model * vec4(vPosition, 1);

    fTextureCoords = vTextureCoords;


    // this is supposed to generate a fog value that gets higher as the distance gets further
    vec4 worldPosition = model * vec4(vPosition, 1);
    vec4 positionRelativeToCamera = view * worldPosition;

    float distance = length( positionRelativeToCamera.xyz );
    visibility = exp( -pow((distance * density), gradient) );
    visibility = clamp(visibility, 0.f, 1.f);
}
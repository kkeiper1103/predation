#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vTexture;
layout(location = 2) in vec3 vNormal;

#include "_uniforms.glsl"

out vec3 fPosition;
out vec3 fTexture;
out vec3 fNormal;
out float visibility;

const float density = 0.0135;
const float gradient = 5.0;

void main() {

    gl_Position = projection * view * model * vec4(vPosition, 1);

    fPosition = vec3(model * vec4(vPosition, 1));
    fTexture = vTexture;
    fNormal = mat3(transpose(inverse(model))) * vNormal;

    // this is supposed to generate a fog value that gets higher as the distance gets further
    vec4 worldPosition = model * vec4(vPosition, 1);
    vec4 positionRelativeToCamera = view * worldPosition;

    float distance = length( positionRelativeToCamera.xyz );
    visibility = exp( -pow((distance * density), gradient) );
    visibility = clamp(visibility, 0.f, 1.f);
}
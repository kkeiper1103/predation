#version 430 core

#include "_uniforms.glsl"

in vec3 fPosition;
in vec3 fTexture;
in vec3 fNormal;
in float visibility;

uniform sampler2DArray terrain;

out vec4 FragColor;


void main() {
    FragColor = texture(terrain, fTexture);

    // FragColor = applyLighting( texture(terrain, coords), fNormal, fPosition );

    // generates a fog. keep in sync with Mesh.frag
    // @todo make include with this stuff or something
    vec4 fogColor = vec4(.7, .7, .7, .7);
    // FragColor = mix(fogColor, FragColor, visibility);
}
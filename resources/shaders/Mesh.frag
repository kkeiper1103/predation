#version 430 core

in vec2 fTextureCoords;
in float visibility;

#include "_uniforms.glsl"

uniform sampler2D diffuse;
uniform sampler2D specular;

out vec4 FragColor;

void main() {
    FragColor = texture(diffuse, fTextureCoords);

    if(FragColor.a < 0.05) discard;
    else {

        // generates a fog. keep in sync with Mesh.frag
        // @todo make include with this stuff or something
        vec4 fogColor = vec4(.7, .7, .7, .7);
        FragColor = mix(fogColor, FragColor, visibility);
    }
}
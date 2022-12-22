#version 430 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

#include "_uniforms.glsl"

void main()
{
    TexCoords = aPos;

    // removes the player position translation from the view matrix
    mat4 untranslatedView = mat4(mat3(view));

    TexCoords = aPos;
    vec4 pos = projection * untranslatedView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
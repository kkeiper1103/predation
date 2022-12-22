#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

#include "_uniforms.glsl"

out vec2 TexCoords;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1);

    TexCoords = aTex;

    /**TexCoords = aPos;

    // removes the player position translation from the view matrix
    mat4 untranslatedView = mat4(mat3(view));

    TexCoords = aPos;
    vec4 pos = projection * untranslatedView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;*/
}
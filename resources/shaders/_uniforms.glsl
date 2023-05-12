// use uniform buffer to keep shared uniforms in sync between shaders
layout(std140, binding=0) uniform matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model = mat4(1.f);

uniform vec3 viewPosition = vec3(0);

#include "_lighting.glsl"
uniform Light lights[32];

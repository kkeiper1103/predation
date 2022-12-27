// @todo switch to UBO block

uniform mat4 projection = mat4(1.f);
uniform mat4 view = mat4(1.f);
uniform mat4 model = mat4(1.f);

uniform vec3 viewPosition = vec3(0);

#include "_lighting.glsl"
uniform Light lights[32];
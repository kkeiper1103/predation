struct Light {
    vec3 position;

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;

    float attenuation;
};

Light theLight;

vec3 calcDiffuseLighting(in Light light, in vec3 normal, in vec3 position) {
    vec3 lightDirection = normalize(light.position - position);
    float diff = max(dot(normal, lightDirection), 0.f);
    vec3 diffuse = diff * light.diffuse;

    return diffuse;
}

vec3 calcSpecularLighting(in Light light, in vec3 normal, in vec3 position) {
    vec3 lightDirection = normalize(light.position - position);

    // probably a bad idea, but this is included after the uniforms file, so this should work
    float specularStrength = .5;
    vec3 viewDir = normalize(viewPosition - position);
    vec3 reflectDir = reflect(-lightDirection, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.f), 16.f);
    vec3 specular = specularStrength * spec * light.specular;

    return specular;
}

vec3 calcAmbientLighting(in Light light) {
    float ambientStrength = .6;

    return ambientStrength * light.ambient;
}

vec4 applyLighting(in vec4 frag, in vec3 normal, in vec3 position) {
    theLight.position = vec3(256, 256, 256);
    theLight.diffuse = vec3(.5, .5, .5);
    theLight.specular = vec3(1, 1, 1);
    theLight.ambient = vec3(.91, .94, .87);
    theLight.attenuation = .8;

    vec3 result = (calcAmbientLighting(theLight) + calcDiffuseLighting(theLight, normal, position) + calcSpecularLighting(theLight, normal, position)) * frag.rgb;

    return vec4(result, frag.a);
}
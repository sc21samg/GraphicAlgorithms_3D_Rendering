#version 430

in vec3 v2fPosition;
in vec3 v2fColor;
in vec3 v2fNormal;
in vec2 v2fTexCoord; // Texture coordinates from vertex shader

layout (location = 0) out vec3 oColor;

// task 1.6
in vec3 v2fWorldPosition; // Received from vertex shader
// Add uniforms for point light properties. Assume three point lights
uniform vec3 pointLightPositions[3];
uniform vec3 pointLightColors[3];
uniform vec3 uCameraPosition; // Uniform for the camera position

// Uniforms for lighting calculations
 layout(location = 2) uniform vec3 uLightDir;     // Directional light direction
 layout(location = 3) uniform vec3 uLightDiffuse; // Diffuse illumination
 layout(location = 4) uniform vec3 uSceneAmbient; // Scene ambient illumination
 uniform sampler2D uTexture; // Texture sampler uniform

void main() {
    vec3 normal = normalize(v2fNormal);
    vec3 lightDir = normalize(uLightDir);
    // Compute clamped dot product of normal and light direction
    float nDotL = max(0.0, dot(normal, lightDir));
    // Sample the texture using texture coordinates
    vec3 textureColor = texture(uTexture, v2fTexCoord).rgb; // Sample the texture
    // Calculate final color using simplified Blinn-Phong model and texture
    oColor = (uSceneAmbient + nDotL * uLightDiffuse) * textureColor * v2fColor;


       // task1.6
    vec3 viewDir = normalize(uCameraPosition - v2fWorldPosition); // Calculate view direction
    // test if it works
    // Ambient term
    vec3 ambient = uSceneAmbient * textureColor * v2fColor;
    vec3 lightContribution = vec3(0.0, 0.0, 0.0);

    // Iterate over the three point lights
    for (int i = 0; i < 3; ++i) {
        // Direction from the current fragment to the point light
        vec3 lightDir = normalize(pointLightPositions[i] - v2fWorldPosition);

        // Calculate the distance between the point light and the fragment
        float distance = length(pointLightPositions[i] - v2fWorldPosition);

        // Calculate attenuation based on distance
        float attenuation = 1.0 / (distance * distance);

        // Diffuse component
        float diff = max(dot(v2fNormal, lightDir), 0.0);

        // Specular component
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(v2fNormal, halfwayDir), 0.0), 32.0); // 32.0 is the shininess factor

        // Combined lighting for the current light
        vec3 computedIllumination = diff * pointLightColors[i] + spec * pointLightColors[i];

        // Accumulate light contribution, attenuated by distance
        lightContribution += computedIllumination * attenuation;
    }

    // Combine ambient and accumulated light contributions, modulated by texture and vertex color
    //oColor = ambient + lightContribution * textureColor * v2fColor;
}
#version 330 core
#define N_STEPS 64
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2DShadow depthMap;

uniform mat4 lightSpaceMatrix;
uniform vec3 viewPos;

float shadowCalc(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;

    projCoords.z -= 0.002; 
    return 1 - texture(depthMap, projCoords);        
}

void main()
{
    vec2 texCoords = TexCoords / textureSize(texture1, 0);
    vec4 color = texture(texture1, texCoords);
    if (color.a < 0.1)
        discard;
    vec3 dist = viewPos - FragPos;
    dist /= N_STEPS;
    float sum = 0;
    for (int i = 1; i <= N_STEPS; i++) {
        vec3 currentPos = FragPos + dist * i;
        sum += shadowCalc(lightSpaceMatrix * vec4(currentPos, 1.0));
    }
    sum /= N_STEPS;
    vec3 shadowColor = vec3(sum);
    FragColor = vec4(shadowColor, 1);
} 
#version 330 core
#extension GL_ARB_arrays_of_arrays : require

#define N_STEPS 24
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2DShadow depthMap;

uniform mat4 lightSpaceMatrix;
uniform vec3 viewPos;

const float ditter[][] = float[][](
    float[]( 0.0f, 0.5f, 0.125f, 0.625f),
    float[]( 0.75f, 0.22f, 0.875f, 0.375f),
    float[]( 0.1875f, 0.6875f, 0.0625f, 0.5625),
    float[]( 0.9375f, 0.4375f, 0.8125f, 0.3125));

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
    vec3 rayStep = viewPos - FragPos;
    rayStep /= N_STEPS;
    vec3 currentPos = FragPos;
    float sum = 0;

    currentPos += rayStep * ditter[int(gl_FragCoord.x)% 4][int(gl_FragCoord.y)% 4];

    for (int i = 0; i <= N_STEPS; i++) {
        sum += shadowCalc(lightSpaceMatrix * vec4(currentPos, 1.0));
        currentPos += rayStep;
    }
    sum /= N_STEPS;
    vec3 shadowColor = vec3(sum);
    FragColor = vec4(shadowColor, 1);
} 
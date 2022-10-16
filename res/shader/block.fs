#version 330 core
out vec4 FragColor;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D texture1;
uniform samplerCube skybox;
uniform sampler2DShadow depthMap;

uniform Light light;
uniform vec3 viewPos;

const vec2 poissonDisk[4] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 )
);

float shadowCalc(const vec4 fragPosLightSpace, const vec3 normal, const vec3 lightDir, const vec2 texelSize) {
    if (fragPosLightSpace.z > 1.0)
        return 0.0;
    float intensity = exp(-pow(length(vec2(0.5) - fragPosLightSpace.xy) * 2.2, 15.0));
    if (intensity < 0)
        return 0.0;
    float shadow = 0;
    vec3 shadowCoords = fragPosLightSpace.xyz;

    for (int i = 0; i < 4; i++) {
        shadowCoords.xy = fragPosLightSpace.xy + poissonDisk[i] * texelSize;
        shadow += texture(depthMap, shadowCoords);
    }

    return intensity * (1 - shadow/4);
}

void main()
{
    vec2 texCoords = TexCoords / textureSize(texture1, 0);
    vec4 texColor = texture(texture1, texCoords);

    if (texColor.a < 0.1)
        discard;

    // ambient
    vec3 ambient = light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 offset = normalize(texColor.rgb) * 0.01;
    float angle = dot(viewDir - offset, reflectDir);
    float spec = pow(max(angle, 0.0), 32.0f);
    vec3 specular = light.specular * spec;  

    float dis = length(viewPos - FragPos);
    float fog = exp(-pow((dis * 0.012), 2.0));
    fog = clamp(fog, 0.4, 1.0);

    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    float shadow = shadowCalc(FragPosLightSpace, norm, lightDir, texelSize);

    vec3 result = ambient + (1.0 - shadow) * (specular + diffuse);
    result *= texColor.rgb;

    vec3 fogTexture = vec3(0.8, 0.9, 1.0);
    result = result * fog + fogTexture * (1 - fog);
    
    // gamma correction 
    float gamma = 0.7;
    result = pow(result, vec3(1.0 / gamma));
  
    FragColor = vec4(result, texColor.a);
} 

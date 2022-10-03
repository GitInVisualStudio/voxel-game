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
uniform sampler2D depthMap;

uniform Light light;
uniform vec3 viewPos;

const float kernel[] = float[] (
    1.0/16.0, 1.0/8.0, 1.0/16.0,
    1.0/8.0, 1.0/4.0, 1.0/16.0,
    1.0/15.0, 1.0/8.0, 1.0/16.0
);

float shadowCalc(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;

    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001); 
    float currentDepth = projCoords.z;
    float shadow = 0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    float intensity = exp(-pow(length(vec2(0.5) - projCoords.xy) * 2.2, 15.0));
    if (intensity < 0)
        return 0.0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float strenght = kernel[(x + 1) * 3 + y + 1];
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += (currentDepth - bias > pcfDepth ? 1.0 : 0.0) * strenght;        
        }    
    }
    return shadow * intensity;
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

    float shadow = shadowCalc(FragPosLightSpace, norm, lightDir);

    vec3 result = ambient + (1.0 - shadow) * (specular + diffuse);
    result *= texColor.rgb;

    vec3 fogTexture = vec3(0.8, 0.9, 1.0);
    result = result * fog + fogTexture * (1 - fog);
    
    // gamma correction 
    float gamma = 0.7;
    result = pow(result, vec3(1.0 / gamma));
  
    FragColor = vec4(result, texColor.a);
} 
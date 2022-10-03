#version 330 core
out vec4 FragColor;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const float amplitude = 0.08;
const float period = 0.5;

in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D texture1;
uniform samplerCube skybox;
uniform sampler2D depthMap;
uniform sampler2D reflectionMap;
uniform sampler2D dudvMap;
uniform float time;

uniform Light light;
uniform vec3 viewPos;
uniform vec2 screenSize;

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

    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.002); 
    float currentDepth = projCoords.z;
    float shadow = 0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    float intensity = exp(-pow(length(vec2(0.5) - projCoords.xy) * 2, 10.0));
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

    vec2 distortion = texture(dudvMap, vec2(FragPos.x + time, FragPos.z) * 0.02).rg;
    distortion = distortion * 2 - 1;
    distortion *= 0.003;


    vec2 distortion2 = texture(dudvMap, FragPos.xz * 0.02 + distortion).rg;
    distortion2 = distortion2 * 2 - 1;
    distortion2 *= 0.003;

    vec3 norm = vec3(0, 1, 0);
    norm.xz += distortion2 * 8;
    norm = normalize(norm);

    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;  

    vec3 specNorm = vec3(0, 1, 0);
    specNorm.xz += distortion2 * 20; 
    specNorm = normalize(specNorm);

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, specNorm);  
    float angle = dot(viewDir, reflectDir);
    float spec = pow(max(angle, 0.0), 32.0f);
    vec3 specular = light.specular * spec;  

    float dis = length(viewPos - FragPos);
    float fog = exp(-pow((dis * 0.012), 2.0));
    fog = clamp(fog, 0.4, 1.0);

    float shadow = shadowCalc(FragPosLightSpace, norm, lightDir);

    vec3 result = ambient + (1.0 - shadow) * diffuse;
    result *= texColor.rgb;

    vec2 uv = gl_FragCoord.xy / screenSize;
    vec2 refTexCoords = uv + distortion;
    refTexCoords = clamp(refTexCoords, 0.001, 0.999);

    result *= texture(reflectionMap, refTexCoords).rgb * 0.8;
    result += texture(skybox, reflect(-viewDir, norm)).rgb * 0.4;

    vec3 fogTexture = vec3(0.8, 0.9, 1.0);
    result = result * fog + fogTexture * (1 - fog);
    
    // gamma correction 
    float gamma = 0.7;
    result = pow(result, vec3(1.0 / gamma));
  
    FragColor = vec4(result, texColor.a) + vec4(specular, 0);
} 
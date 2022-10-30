#version 330 core
out vec4 FragColor;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const float distortion_strength = 0.003;
const float distortion_scale = 0.02;

in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform samplerCube skybox;
uniform sampler2D reflectionMap;
uniform sampler2D dudvMap;
uniform float time;

uniform Light light;
uniform vec3 viewPos;
uniform vec2 screenSize;

void main()
{
    vec2 texCoords = TexCoords / textureSize(texture1, 0);
    vec4 texColor = texture(texture1, texCoords);

    if (texColor.a < 0.1)
        discard;

    // ambient
    vec3 ambient = light.ambient;

    vec2 distortion = texture(dudvMap, vec2(FragPos.x + time, FragPos.z) * distortion_scale).rg;
    distortion = distortion * 2 - 1;
    distortion *= distortion_strength;


    vec2 distortion2 = texture(dudvMap, FragPos.xz * distortion_scale + distortion).rg;
    distortion2 = distortion2 * 2 - 1;
    distortion2 *= distortion_strength;

    vec3 norm = vec3(0, 1, 0);
    norm.xz += distortion2 * 8;
    norm = normalize(norm);

    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;  


    vec2 distortion3 = texture(dudvMap, FragPos.xz * 0.05 + distortion * 5).rg;
    distortion3 = distortion3 * 2 - 1;
    distortion3 *= distortion_strength;
    vec3 specNorm = vec3(0, 1, 0);
    specNorm.xz += distortion2 * 20 + distortion3 * 5; 
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

    vec3 result = ambient + diffuse;
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
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

uniform sampler2D texture1;

uniform Light light;
uniform vec3 viewPos;

void main()
{
    vec2 texCoords = TexCoords / textureSize(texture1, 0);
    vec4 texColor = texture(texture1, texCoords);

    if (texColor.a < 0.1 || FragPos.y >= 19)
        discard;

    // ambient
    vec3 ambient = light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;  

    vec3 result = ambient*2 + diffuse;
    result *= texColor.rgb;
    
    // gamma correction 
    float gamma = 0.7;
    result = pow(result, vec3(1.0 / gamma));
  
    FragColor = vec4(result, texColor.a);
} 
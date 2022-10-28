
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D volumeMap;

void main()
{             
    float volumeValue = texture(volumeMap, TexCoords).r;
    volumeValue = 1 - volumeValue;
    volumeValue = pow(volumeValue, 5);
    volumeValue = max(volumeValue, 0.15);
    volumeValue = 1 - volumeValue;
    FragColor = vec4(vec3(0), volumeValue);
}


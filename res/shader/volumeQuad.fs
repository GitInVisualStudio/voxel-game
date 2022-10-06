
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D volumeMap;

const int size = 2;
const float kernel[] = float[] (
    1 / 256.0, 4  / 256.0,  6 / 256.0,  4 / 256.0, 1 / 256.0,
    4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0,
    6 / 256.0, 24 / 256.0, 36 / 256.0, 24 / 256.0, 6 / 256.0,
    4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0,
    1 / 256.0, 4  / 256.0,  6 / 256.0,  4 / 256.0, 1 / 256.0
);

void main()
{             
    float volumeValue = 0;
    vec2 tileSize = 1.0 / textureSize(volumeMap, 0);
    for (int x = -size; x <= size; x++) {
        for (int y = -size; y <= size; y++) {
            volumeValue += texture(volumeMap, TexCoords + vec2(x, y) * tileSize).r * kernel[(x + size) * 5 + y + size];
        }
    }
    volumeValue = 1 - volumeValue;
    volumeValue = pow(volumeValue, 5);
    volumeValue = max(volumeValue, 0.15);
    FragColor = vec4(vec3(0), 1 - volumeValue);
}


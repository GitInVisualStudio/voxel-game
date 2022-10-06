
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorMap;

const float kernel[] = float[] (
    1 / 256.0, 4  / 256.0,  6 / 256.0,  4 / 256.0, 1 / 256.0,
    4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0,
    6 / 256.0, 24 / 256.0, 36 / 256.0, 24 / 256.0, 6 / 256.0,
    4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0,
    1 / 256.0, 4  / 256.0,  6 / 256.0,  4 / 256.0, 1 / 256.0
);

vec4 getColor(vec2 coords) {
    vec3 color = texture(colorMap, coords).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    return brightness > 0.5 ? vec4(color, 1.0) : vec4(color, 0.0);
}

void main()
{             
    vec4 color = vec4(0);
    vec2 tileSize = 1.0 / textureSize(colorMap, 0);
    const int size = 2;
    for (int x = -size; x <= size; x++) {
        for (int y = -size; y <= size; y++) {
            color += getColor(TexCoords + vec2(x, y) * tileSize) * kernel[(x + size) * 5 + y + size];
        }
    }
    FragColor = color;
}
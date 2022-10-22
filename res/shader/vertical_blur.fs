#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D inputTexture;

const int size = 5;
const float kernel[] = float[] (1.0 / 16, 4.0 / 16, 6.0 / 16, 4.0 / 16, 1.0 / 16);

void main()
{             
    vec4 color = vec4(0);
    vec2 texelSize = 1.0 / textureSize(inputTexture, 0);
    for (int i = 0; i < size; i++)
        color += texture(inputTexture, TexCoords + vec2(0, i - 2) * texelSize) * kernel[i];
    FragColor = color;
    // FragColor = vec4(TexCoords.x, TexCoords.y, 0, 1);
}


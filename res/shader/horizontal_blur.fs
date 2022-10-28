
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D inputTexture;

const int size = 5;
const float kernel[] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec4 color = texture(inputTexture, TexCoords) * kernel[0];
    vec2 texelSize = 1.0 / textureSize(inputTexture, 0);
    for (int i = 1; i < size; i++) {
        color += texture(inputTexture, TexCoords + vec2(i, 0) * texelSize) * kernel[i];
        color += texture(inputTexture, TexCoords - vec2(i, 0) * texelSize) * kernel[i];

    }
        
    FragColor = color;
    // FragColor = vec4(TexCoords.x, TexCoords.y, 0, 1);
}


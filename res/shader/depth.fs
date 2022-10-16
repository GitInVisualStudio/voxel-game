#version 330 core

in vec2 TexCoords;
uniform sampler2D texture1;

void main()
{
    vec2 texCoords = TexCoords / textureSize(texture1, 0);
    vec4 color = texture(texture1, texCoords);
    if (color.a < 0.1)
        discard;
    // vec2 uv = gl_FragCoord.xy / screenSize;
    // float prevDepth = texture(backbuffer, uv).r;
    // const float rateOfChange = 0.2;
    // gl_FragDepth = prevDepth * rateOfChange + gl_FragCoord.z * (1 - rateOfChange);
} 
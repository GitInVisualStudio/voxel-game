
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorMap;
uniform float threshold;

vec3 prefilter (vec3 c) {
    float brightness = max(c.r, max(c.g, c.b));
    float contribution = max(0, brightness - threshold);
    contribution /= max(brightness, 0.00001);
    return c * contribution;
}

void main()
{             
    FragColor = vec4(prefilter(texture(colorMap, TexCoords).rgb), 1.0);
}
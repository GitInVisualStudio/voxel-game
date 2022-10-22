
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float alpha;

void main()
{             
    FragColor = texture(depthMap, TexCoords);
    FragColor.a = alpha;
}


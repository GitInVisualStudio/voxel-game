#version 330 core

layout (location = 0) in uint aData;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
out vec2 TexCoords;

uint getData(uint offset, uint size) {
    return (aData >> offset) & size;
}

void main() {
    uint xPos = getData(0u, 31u);
    uint yPos = getData(5u, 63u);
    uint zPos = getData(11u, 31u);
    vec3 aPos = vec3(xPos, yPos, zPos);
    TexCoords = vec2(getData(19u, 127u), getData(26u, 127u));   
    vec4 pos = lightSpaceMatrix * model * vec4(aPos, 1.0);
    gl_Position = pos;
}
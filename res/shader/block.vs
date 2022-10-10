#version 330 core

layout (location = 0) in uint aData;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uint getData(uint offset, uint size) {
    return (aData >> offset) & size;
}

const vec3 normals[6] = vec3[](    
    vec3(0, 1, 0),
    vec3(0, -1, 0),
    vec3(-1, 0, 0),
    vec3(1, 0, 0),
    vec3(0, 0, 1),
    vec3(0, 0, -1)
);

const float epsilon = 0.0005;

void main() {
    uint xPos = getData(0u, 31u);
    uint yPos = getData(5u, 63u);
    uint zPos = getData(11u, 31u);
    vec3 aPos = vec3(xPos, yPos, zPos);
    vec3 aNormal = normals[getData(16u, 7u)];

    TexCoords = vec2(getData(19u, 127u), getData(26u, 127u));
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    FragPosLightSpace.xyz /= FragPosLightSpace.w;
    FragPosLightSpace.xyz = FragPosLightSpace.xyz * 0.5 + 0.5;
    FragPosLightSpace.z -= epsilon;
    vec4 pos = projection * view * model * vec4(aPos, 1.0);
    gl_Position = pos;
}
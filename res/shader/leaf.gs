#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 27) out;

const int res = 2;
const float amplitude = 0.07;
const float period = 1.0;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float time;

vec4 getPosition(int index, int x, int y, vec4 dif) {

    if (dif.y == 0) {
        dif.x *= x;
        dif.z *= y;
    } else {
        dif.x *= x;
        dif.z *= x;
        dif.y *= y;
    }

    vec4 origin = gl_in[0].gl_Position;
    vec4 pos = gl_in[index].gl_Position;
    vec4 dir = origin - pos;
    dir /= res;
    vec4 result = origin - dir - dif;

    float amp = cos(time) * amplitude + amplitude;

    result.x += sin(result.y * period + time) * amp;
    result.z += sin(result.y * period + time) * amp;

    return result;
}

void main() {
    vec4 dif = (gl_in[0].gl_Position - gl_in[1].gl_Position) + (gl_in[0].gl_Position - gl_in[2].gl_Position);
    if (abs(dif.x) == 2) 
        dif.x /= 2;
    if (abs(dif.y) == 2) 
        dif.y /= 2;
    if (abs(dif.z) == 2) 
        dif.z /= 2;
    dif /= res;

    for (int x = 0; x < res; x++) {
        for (int y = 0; y < res; y++) {
            gl_Position = projection * view * model * getPosition(0, x, y, dif);
            EmitVertex();
            gl_Position = projection * view * model * getPosition(1, x, y, dif);
            EmitVertex();
            gl_Position = projection * view * model * getPosition(2, x, y, dif);
            EmitVertex();
            EndPrimitive();
        }
    }
} 
#version 300 es

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUV;

out vec2 outUV;

void main()
{
    gl_Position  = vec4(vPosition, 1.0);
    outUV = vec2(vUV.x, vUV.y);
}
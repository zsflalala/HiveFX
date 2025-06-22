#version 300 es
precision mediump float;

uniform sampler2D uSourceTex;

in vec2 vTexCoord;
out vec4 oFragColor;

void main()
{
    oFragColor = texture(uSourceTex, vTexCoord);
}
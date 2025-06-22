#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D uIndexTexture;
uniform sampler2D uEraseMask;
uniform int       uChannelIndex;

void main()
{
    vec4 indexColor = texture(uIndexTexture, vTexCoord);
    float erase = texture(uEraseMask, vTexCoord).r;
    float index = indexColor[uChannelIndex];
    float alpha = index * erase;
    FragColor = vec4(1.0, 1.0, 1.0, alpha);
}
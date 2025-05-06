#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D indexTexture;
uniform int channelIndex;

void main()
{
    vec4 indexColor = texture(indexTexture, TexCoord);
    FragColor = indexColor;
}
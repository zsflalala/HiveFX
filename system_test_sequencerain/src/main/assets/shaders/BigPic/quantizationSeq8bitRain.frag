#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform vec2 texUVOffset;
uniform vec2 texUVScale;
uniform sampler2D indexTexture;
uniform int channelIndex;

void main()
{
    vec2 TexCoords = (TexCoord * texUVScale) + texUVOffset;

    vec4 indexColor = texture(indexTexture, TexCoords);
    float index = 0.0;
    if (channelIndex == 0)      index = indexColor.r;
    else if (channelIndex == 1) index = indexColor.g;
    else if (channelIndex == 2) index = indexColor.b;
    else if (channelIndex == 3) index = indexColor.a;

    FragColor = vec4(index, index, index, 0.15f);
}
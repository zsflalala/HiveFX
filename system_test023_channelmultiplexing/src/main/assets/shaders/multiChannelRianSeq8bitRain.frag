#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D indexTexture;
uniform int channelIndex;

void main()
{
    vec4 indexColor = texture(indexTexture, TexCoord);

    float index = 0.0;
    if (channelIndex == 0)
    {
        index = indexColor.r;
    }
    else if (channelIndex == 1)
    {
        index = indexColor.r + indexColor.g;
    }
    else if (channelIndex == 2)
    {
        index = indexColor.r + indexColor.g + indexColor.b;
    }
    else if (channelIndex == 3)
    {
        index = indexColor.r + indexColor.g + indexColor.b + indexColor.a;
    }
    FragColor = vec4(index, index, index, 0.1);
}
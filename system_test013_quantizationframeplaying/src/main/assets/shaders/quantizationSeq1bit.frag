#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D indexTexture;
uniform int channelIndex;

void main()
{
    vec4 packedData = texture(indexTexture, TexCoord);
    uvec4 color = uvec4(packedData * 255.0);

    int channel = channelIndex / 8;                             // 第几通道: 0-R, 1-G, 2-B, 3-A
    int shift = 7 - (channelIndex % 8);                         // 从高位开始取（7 ~ 0）

    uint value = 0u;
    if (channel == 0)      value = (color.r >> shift) & 1u;
    else if (channel == 1) value = (color.g >> shift) & 1u;
    else if (channel == 2) value = (color.b >> shift) & 1u;
    else if (channel == 3) value = (color.a >> shift) & 1u;

    float brightness = float(value);
    if (brightness < 1.0)
        discard;
    FragColor = vec4(vec3(brightness), 1.0);
}
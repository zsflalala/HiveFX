#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform float Factor;
uniform float Displacement;
uniform int ChannelIndex;
uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;

void main()
{
    float CurrentTexOffset = TexCoord.x - Displacement * Factor;
    vec4 CurrentColorAll = texture(CurrentTexture, vec2(CurrentTexOffset, TexCoord.y));
    float CurrentColor = float[4](CurrentColorAll.r, CurrentColorAll.g, CurrentColorAll.b, CurrentColorAll.a)[ChannelIndex];
    float NextTextureOffset = TexCoord.x + Displacement * (1.0 - Factor);
    vec4 NextColorAll = texture(NextTexture, vec2(NextTextureOffset, TexCoord.y));
    float NextColor = float[4](NextColorAll.r, NextColorAll.g, NextColorAll.b, NextColorAll.a)[ChannelIndex];
    float MixColor = mix(CurrentColor, NextColor, Factor);
    FragColor = vec4(1.0, 1.0, 1.0, MixColor);
}
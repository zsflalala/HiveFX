#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform float Factor;
uniform float Displacement;
uniform int CurrentChannel;
uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;

void main()
{
    float CurrentTexOffset = TexCoord.x - Displacement * Factor;
    vec4 CurrentColorAll = texture(CurrentTexture, vec2(CurrentTexOffset, TexCoord.y));
    float CurrentColor = CurrentColorAll[CurrentChannel];

    int NextChannel = (CurrentChannel + 1) % 4;
    float NextTextureOffset = TexCoord.x + Displacement * (1.0 - Factor);
    vec4 NextColorAll = texture(NextTexture, vec2(NextTextureOffset, TexCoord.y));
    float NextColor = NextColorAll[NextChannel];

    float MixColor = mix(CurrentColor, NextColor, Factor);
    FragColor = vec4(1.0, 1.0, 1.0, MixColor);
}
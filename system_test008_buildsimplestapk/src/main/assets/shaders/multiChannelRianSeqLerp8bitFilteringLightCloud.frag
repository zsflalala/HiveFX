#version 300 es
precision mediump float;

in vec2 TexCoord;

uniform float Factor;
uniform float Displacement;
uniform int CurrentChannel;
uniform vec2 TexelSize;
uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;

out vec4 FragColor;

// 简单3x3空间滤波均值（某个纹理某通道）
float filteredChannelSpace3x3(sampler2D vTex, vec2 vUV, int vChannelIndex)
{
    float Sum = 0.0;
    float Count = 0.0;
    float FilterSpacingFactor = 3.0;

    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            vec2 SampleUV = vUV + vec2(float(dx), float(dy)) * TexelSize * FilterSpacingFactor;
            vec4 SampleColor = texture(vTex, SampleUV);
            Sum += SampleColor[vChannelIndex];
            Count += 1.0;
        }
    }
    return Sum / Count;
}

void main()
{
    float CurrentOffset = TexCoord.x - Displacement * Factor;
    vec2  CurrentUV = vec2(CurrentOffset, TexCoord.y);
    float NextOffset = TexCoord.x + Displacement * (1.0 - Factor);
    vec2  NextUV = vec2(NextOffset, TexCoord.y);
    int   NextChannel = (CurrentChannel + 1) % 4;

    float CurrentSpaceFilterColor = filteredChannelSpace3x3(CurrentTexture, CurrentUV, CurrentChannel);
    float NextSpaceFilterColor = filteredChannelSpace3x3(NextTexture, NextUV, NextChannel);

    float MixColor = mix(CurrentSpaceFilterColor, NextSpaceFilterColor, Factor) + 0.1f;
    FragColor = vec4(1.0, 1.0, 1.0, MixColor);
}


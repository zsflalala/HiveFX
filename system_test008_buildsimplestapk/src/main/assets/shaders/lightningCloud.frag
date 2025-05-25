#version 300 es
precision mediump float;

in vec2 TexCoordCloud;
in vec2 TexCoordLightning;

uniform float Factor;
uniform float Displacement;
uniform int   CurrentChannel;
uniform vec2  TexelSize;
uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;

uniform float flashProgress;
uniform vec3  flashColor;
uniform float flashAlpha;
uniform bool  lightningInFront;
uniform int   channelIndex;
uniform sampler2D lightningSequenceTexture;

out vec4 FragColor;

// 3x3 空间滤波，你原来的名称保持不变
float filteredChannelSpace3x3(sampler2D vTex, vec2 vUV, int vChannelIndex)
{
    float Sum = 0.0;
    float Count = 0.0;
    float FilterSpacingFactor = 3.0;

    for (int dx = -1; dx <= 1; ++dx)
        for (int dy = -1; dy <= 1; ++dy)
        {
            vec2 SampleUV = vUV + vec2(float(dx), float(dy)) * TexelSize * FilterSpacingFactor;
            vec4 SampleColor = texture(vTex, SampleUV);
            Sum += SampleColor[vChannelIndex];
            Count += 1.0;
        }
    return Sum / Count;
}

// remap 函数保持原来命名
vec3 remap(vec3 vVector, float vOldMin, float vOldMax, float vNewMin, float vNewMax)
{
    return (vVector - vOldMin) / (vOldMax - vOldMin) * (vNewMax - vNewMin) + vNewMin;
}

float remap(float vData, float vOldMin, float vOldMax, float vNewMin, float vNewMax)
{
    return (vData - vOldMin) / (vOldMax - vOldMin) * (vNewMax - vNewMin) + vNewMin;
}

void main()
{
    // —— 云序列帧插值 + 滤波 ——
    float CurrentOffset = TexCoordCloud.x - Displacement * Factor;
    vec2  CurrentUV     = vec2(CurrentOffset, TexCoordCloud.y);
    float NextOffset    = TexCoordCloud.x + Displacement * (1.0 - Factor);
    vec2  NextUV        = vec2(NextOffset,   TexCoordCloud.y);
    int   NextChannel   = (CurrentChannel + 1) % 4;

    float CurrentSpaceFilterColor = filteredChannelSpace3x3(CurrentTexture, CurrentUV,   CurrentChannel);
    float NextSpaceFilterColor    = filteredChannelSpace3x3(NextTexture,    NextUV,      NextChannel);
    float MixColor = mix(CurrentSpaceFilterColor, NextSpaceFilterColor, Factor) + 0.1f;

    // 把插值结果当作“云”alpha，用白色背景
    vec4 CloudColor = vec4(1.0, 1.0, 1.0, MixColor);

    // —— 闪电遮罩叠加 ——
    // remap 云色底色
    vec3 CloudColorWithoutLight = remap(CloudColor.rgb, 0.0, 1.0, 0.0, 0.5);

    // 取闪电序列帧对应通道
    vec4 LightningColor = texture(lightningSequenceTexture, TexCoordLightning);
    float LightningMask = LightningColor[channelIndex];

    // 云后效果
    CloudColor.rgb = mix(CloudColorWithoutLight, CloudColor.rgb, LightningMask);
    vec4 ColorWhenBehind = CloudColor;

    // —— 全屏闪电提亮 ——
    float UP   = smoothstep(0.0, 0.5, flashProgress);
    float Down = 1.0 - smoothstep(0.5, 1.0, flashProgress);
    float FlashIntensity = UP * Down;

    vec3 FinalLitColor = mix(CloudColor.rgb, flashColor, FlashIntensity * flashAlpha);
    vec4 ColorWhenInFront = vec4(FinalLitColor, 1.0);

    // 根据 lightningInFront 决定前后叠加
    FragColor = mix(ColorWhenBehind, ColorWhenInFront, float(lightningInFront));
}

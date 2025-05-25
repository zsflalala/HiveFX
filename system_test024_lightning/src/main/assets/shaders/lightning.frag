#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoordCloud;
in vec2 TexCoordLightning;

uniform bool lightningInFront;
uniform sampler2D lightningSequenceTexture;
uniform sampler2D cloudTexture;

uniform float uFlashProgress;    // 0～1
uniform vec3  uFlashColor;
uniform float uFlashAlpha;       // 控制最大透明度

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
    vec4 cloud = texture(cloudTexture, TexCoordCloud);
    vec3 dimmed = remap(cloud.rgb, 0.0, 1.0, 0.0, 0.5);

    vec4 lightning = texture(lightningSequenceTexture, clamp(TexCoordLightning, vec2(0.0), vec2(1.0)));
    float lightningMask = lightning.a;
    float lightningIntensity = lightning.r;

    vec3 mixedCloud = mix(dimmed, cloud.rgb, lightningMask);
    vec4 baseCloudColor = vec4(mixedCloud, cloud.a);

    // 前景闪电混合
    vec4 src = vec4(vec3(1.0), lightningIntensity * lightningMask);
    vec4 dst = baseCloudColor;
    float blendAlpha = src.a + dst.a - src.a * dst.a;
    vec3 blendColor = src.rgb * src.a + dst.rgb * dst.a * (1.0 - src.a);
    blendColor /= max(blendAlpha, 0.0001);
    vec4 frontLightningColor = vec4(blendColor, blendAlpha);

    // 后前景混合控制
    float isFront = float(lightningInFront);
    vec4 sceneColor = mix(baseCloudColor, frontLightningColor, isFront);

    // === 关键融合段 ===
    // 计算闪电强度 (先升后降)
    float up   = smoothstep(0.0, 0.5, uFlashProgress);
    float down = 1.0 - smoothstep(0.5, 1.0, uFlashProgress);
    float flashIntensity = up * down;

    // 你的原始方式：用于 mix 提亮（仅在云基础上作用）
    vec3 mixFlashColor = mix(sceneColor.rgb, uFlashColor, flashIntensity * uFlashAlpha);

    // 改进方式：叠加提亮，用于“全屏加亮”效果
    vec3 addFlashColor = sceneColor.rgb + uFlashColor * (flashIntensity * uFlashAlpha);

    // 最终融合方式（二选一或加权）
    // 方式 1：直接使用叠加（推荐用于全屏真实提亮）
    vec3 finalColor = mix(sceneColor.rgb, addFlashColor, isFront);

    // 输出
    FragColor = vec4(finalColor, sceneColor.a);

    // Optional discard
    float discardMask = step(0.0001, FragColor.a);
    FragColor *= discardMask;
}

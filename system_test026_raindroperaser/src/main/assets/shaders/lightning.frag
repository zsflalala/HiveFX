#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoordCloud;
in vec2 TexCoordLightning;

uniform float flashProgress;
uniform vec3  flashColor;
uniform float flashAlpha;
uniform bool lightningInFront;
uniform int channelIndex;
uniform sampler2D lightningSequenceTexture;
uniform sampler2D cloudTexture;

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
    vec4 CloudColor = texture(cloudTexture, TexCoordCloud);
    vec3 CloudColorWithoutLight = remap(CloudColor.rgb, 0.0, 1.0, 0.0, 0.5);

    vec4 LightningColor = texture(lightningSequenceTexture, TexCoordLightning);
    float LightningMask = LightningColor[channelIndex];

    // 云后效果
    CloudColor.rgb = mix(CloudColorWithoutLight, CloudColor.rgb, LightningMask);
    vec4 ColorWhenBehind = CloudColor;

    // 云后效果 + 全屏提亮
    float UP   = smoothstep(0.0, 0.5, flashProgress);
    float Down = 1.0 - smoothstep(0.5, 1.0, flashProgress);
    float FlashIntensity = UP * Down;

    vec3 FinalLitColor = mix(CloudColor.rgb, flashColor, FlashIntensity * flashAlpha);
    vec4 ColorWhenInFront = vec4(FinalLitColor, 1.0);

    FragColor = mix(ColorWhenBehind, ColorWhenInFront, float(lightningInFront));
}
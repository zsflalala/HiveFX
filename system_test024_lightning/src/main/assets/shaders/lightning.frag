#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
uniform vec2 texUVOffset;
uniform vec2 texUVScale;
uniform sampler2D lightningSequenceTexture;
uniform sampler2D cloudTexture;

uniform float uFlashProgress;    // 0～1
uniform vec3  uFlashColor;       // 白光 vec3(1.0)
uniform float uFlashAlpha;       // 控制最大透明度，例如 0.3 表示最多覆盖 30%


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
    vec4 CloudColor = texture(cloudTexture, TexCoord);
    vec3 CloudColorWithoutLight;
    CloudColorWithoutLight = remap(CloudColor.rgb, 0.0, 1.0, 0.0, 0.5);

    vec2 TexCoords = (TexCoord * texUVScale) + texUVOffset;
    vec4 LightningColor = texture(lightningSequenceTexture, TexCoords);

    float LightningMask = LightningColor.r;
    CloudColor.rgb = mix(CloudColorWithoutLight, CloudColor.rgb, LightningMask);

    // 计算闪光强度 (先升后降)
    float up   = smoothstep(0.0, 0.5, uFlashProgress);
    float down = 1.0 - smoothstep(0.5, 1.0, uFlashProgress);
    float flashIntensity = up * down;

    vec3 finalColor = mix(CloudColor.rgb, uFlashColor, flashIntensity * uFlashAlpha);
    FragColor = vec4(finalColor, 1.0);
}

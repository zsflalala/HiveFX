#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
uniform vec2 texUVOffset;
uniform vec2 texUVScale;
uniform sampler2D rainSequenceTexture;
uniform sampler2D backgroundTexture;
uniform int channelIndex;

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
    vec4 BackgroundColor = texture(backgroundTexture, TexCoord);
    float LightMask = BackgroundColor.a;

    vec2 TexCoords = (TexCoord * texUVScale) + texUVOffset;
    vec4 RainColor = texture(rainSequenceTexture, TexCoords);
    float FinalRainColor = RainColor.r
                         + RainColor.g * float(channelIndex >= 1)
                         + RainColor.b * float(channelIndex >= 2)
                         + RainColor.a * float(channelIndex >= 3);

    vec4 SrcColor = vec4(vec3(FinalRainColor), 0.1 + LightMask);

    vec4 DstColor = BackgroundColor;
    DstColor.a = 1.0;

    float BlendAlpha = SrcColor.a + DstColor.a - SrcColor.a * DstColor.a;

    vec3 BlendColor = min(SrcColor.rgb + DstColor.rgb, vec3(1.0, 1.0, 1.0)) * SrcColor.a * DstColor.a;
    BlendColor = BlendColor + SrcColor.rgb * SrcColor.a * (1.0 - DstColor.a) + DstColor.rgb * DstColor.a * (1.0 - SrcColor.a);
    BlendColor = BlendColor / BlendAlpha;
    FragColor = vec4(BlendColor, BlendAlpha);
}
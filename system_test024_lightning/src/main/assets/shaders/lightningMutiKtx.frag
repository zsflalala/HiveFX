#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
uniform vec2 texUVOffset;
uniform vec2 texUVScale;
uniform sampler2D lightningSequenceTexture;
uniform sampler2D cloudTexture;
uniform int channelIndex;


vec3 remap(vec3 vVector, float vOldMin, float vOldMax, float vNewMin, float vNewMax)
{
    return (vVector - vOldMin) / (vOldMax - vOldMin) * (vNewMax - vNewMin) + vNewMin;
}

float remap(float vData, float vOldMin, float vOldMax, float vNewMin, float vNewMax)
{
    return (vData - vOldMin) / (vOldMax - vOldMin) * (vNewMax - vNewMin) + vNewMin;
}
/*
void main()
{
    // 闪电在云前
    vec4 CloudColor = texture(cloudTexture, TexCoord);
    vec3 CloudColorWithoutLight;
    CloudColorWithoutLight = remap(CloudColor.rgb, 0.0, 1.0, 0.0, 0.5);

    vec2 TexCoords = (TexCoord * texUVScale) + texUVOffset;
    vec4 LightningColor = texture(lightningSequenceTexture, TexCoords);

    float LightningMask = LightningColor.a;
    CloudColor.rgb = mix(CloudColorWithoutLight, CloudColor.rgb, LightningMask);

    vec4 SrcColor;
    SrcColor.rgb = vec3(1.0);
    SrcColor.a = LightningColor.r;

    vec4 DstColor = CloudColor;
    // Alpha Blending
    float BlendAlpha = SrcColor.a + DstColor.a - SrcColor.a * DstColor.a;
    if(BlendAlpha < 0.0001)
        discard;
    vec3 BlendColor = SrcColor.rgb * SrcColor.a + DstColor.rgb * DstColor.a * (1.0 - SrcColor.a);
    BlendColor = BlendColor / BlendAlpha;

    FragColor = vec4(BlendColor, BlendAlpha);
}
*/

void main()
{
    //闪电在云后
    vec4 CloudColor = texture(cloudTexture, TexCoord);
    vec3 CloudColorWithoutLight;
    CloudColorWithoutLight = remap(CloudColor.rgb, 0.0, 1.0, 0.0, 0.5);

    vec2 TexCoords = (TexCoord * texUVScale) + texUVOffset;
    vec4 LightningColor = texture(lightningSequenceTexture, TexCoords);

    float LightningMask = LightningColor[channelIndex];
    CloudColor.rgb = mix(CloudColorWithoutLight, CloudColor.rgb, LightningMask);

    FragColor = CloudColor;
}

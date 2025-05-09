#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
uniform vec2 texUVOffset;
uniform vec2 texUVScale;
uniform sampler2D lightningSequenceTexture;
uniform sampler2D cloudTexture;

void main()
{
    vec4 CloudColor = texture(cloudTexture, TexCoord);
    vec2 TexCoords = (TexCoord * texUVScale) + texUVOffset;
    vec4 LightningColor = texture(lightningSequenceTexture, TexCoords);
    vec4 SrcColor = LightningColor;
    vec4 DstColor = CloudColor;
    float BlendAlpha = SrcColor.a + DstColor.a - SrcColor.a * DstColor.a;
    if(BlendAlpha < 0.02)
        discard;
    vec3 BlendColor = min(SrcColor.rgb + DstColor.rgb, vec3(1.0, 1.0, 1.0)) * SrcColor.a * DstColor.a;
    BlendColor = BlendColor + SrcColor.rgb * SrcColor.a * (1.0 - DstColor.a) + DstColor.rgb * DstColor.a * (1.0 - SrcColor.a);
    BlendColor = BlendColor / BlendAlpha;
    FragColor = vec4(BlendColor, BlendAlpha);
}
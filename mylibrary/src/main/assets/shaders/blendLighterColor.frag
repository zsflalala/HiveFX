#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D SrcTexture;
uniform sampler2D DstTexture;

float calcuGray(vec4 vTexel)
{
    float Gray = 0.299 * vTexel.r + 0.587 * vTexel.g + 0.114 * vTexel.b;
    return Gray;
}

void main()
{
    vec4 Source = texture(SrcTexture, TexCoord);
    vec4 Destination = texture(DstTexture, vec2(TexCoord.x, (1.0 - TexCoord.y)));

    float SrcBrightness = calcuGray(Source);
    float DstBrightness = calcuGray(Destination);
    float MainTexMask = step(DstBrightness, SrcBrightness);

    vec4 MainTex = Source * MainTexMask + Destination * (1.0 - MainTexMask);
    vec4 MildTex = Source * (1.0 - MainTexMask) + Destination * MainTexMask;

    FragColor.a = Source.a + Destination.a * ( 1.0 - Source.a );
    FragColor.rgb = MainTex.rgb * MainTex.a + MildTex.rgb * MildTex.a * (1.0 - MainTex.a);
    FragColor.rgb /= FragColor.a;
}
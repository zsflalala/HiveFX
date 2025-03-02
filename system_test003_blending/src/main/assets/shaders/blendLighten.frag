#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D SrcTexture;
uniform sampler2D DstTexture;

void main()
{
    vec4 Source = texture(SrcTexture, TexCoord);
    vec4 Destination = texture(DstTexture, vec2(TexCoord.x, (1.0 - TexCoord.y)));
    vec3 SrcColorWithAlpha = Source.rgb * Source.a;
    vec3 DstColorWithAlpha = Destination.rgb * Destination.a;

    vec3 MainTexMask = step(Destination.rgb, Source.rgb);

    vec3 MainTex = SrcColorWithAlpha * MainTexMask + DstColorWithAlpha * (1.0 - MainTexMask);
    vec3 MildTex = SrcColorWithAlpha * (1.0 - MainTexMask) + DstColorWithAlpha * MainTexMask;

    vec3 MainTexAlpha = Source.a * MainTexMask + Destination.a * vec3(1.0 - MainTexMask);

    FragColor.rgb = MainTex + MildTex * (1.0 - MainTexAlpha);
    FragColor.a = Source.a + Destination.a * ( 1.0 - Source.a );
    FragColor.rgb /= FragColor.a;
}
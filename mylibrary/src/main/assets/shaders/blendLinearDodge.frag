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

    FragColor.a = Source.a + Destination.a * ( 1.0 - Source.a );
    FragColor.rgb = min(Source.rgb + Destination.rgb, vec3(1.0)) * Source.a *  Destination.a + SrcColorWithAlpha * (1.0 - Destination.a) + DstColorWithAlpha * (1.0 - Source.a);
    FragColor.rgb /= FragColor.a;
}
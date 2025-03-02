#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D SrcTexture;
uniform sampler2D DstTexture;

vec3 multiply(vec4 Source, vec4 Destination)
{
    vec3 SrcColor = Source.rgb;
    float SrcAlpha = Source.a;
    vec3 DstColor = Destination.rgb;
    float DstAlpha = Destination.a;
    vec3 Multiply = SrcColor * SrcAlpha * DstColor * DstAlpha;
    vec3 SrcContribution = SrcColor * SrcAlpha * ( 1.0 - DstAlpha );
    vec3 DstContribution = DstColor * DstAlpha * ( 1.0 - SrcAlpha );
    vec3 ResultColor =  Multiply + SrcContribution + DstContribution;
    return ResultColor;
}

vec3 calcuScreen(vec4 Source, vec4 Destination)
{
    vec3 SrcColorWithAlpha = Source.rgb * Source.a;
    vec3 DstColorWithAlpha = Destination.rgb * Destination.a;
    vec3 ResultColor = 1.0 - ( 1.0 - SrcColorWithAlpha ) * ( 1.0 - DstColorWithAlpha );
    return ResultColor;
}

void main()
{
    vec4 Source = texture(SrcTexture, TexCoord);
    vec4 Destination = texture(DstTexture, vec2(TexCoord.x, (1.0 - TexCoord.y)));

    vec3 SrcColorWithAlpha = Source.rgb * Source.a;
    vec3 DstColorWithAlpha = Destination.rgb * Destination.a;

    float IsSrcGreaterThanHalf = step(vec(0.5), SrcColorWithAlpha);

    FragColor.rgb = min(SrcColorWithAlpha + DstColorWithAlpha, vec3(1.0));
    FragColor.a = Source.a + Destination.a * ( 1.0 - Source.a );
}
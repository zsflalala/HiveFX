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
    vec3 SrcColor = Source.rgb;
    float SrcAlpha = Source.a;
    vec3 DstColor = Destination.rgb;
    float DstAlpha = Destination.a;
    FragColor.a = SrcAlpha + DstAlpha * ( 1.0 - SrcAlpha );
    vec3 Multiply = SrcColor * SrcAlpha * DstColor * DstAlpha;
    vec3 SrcContribution = SrcColor * SrcAlpha * ( 1.0 - DstAlpha );
    vec3 DstContribution = DstColor * DstAlpha * ( 1.0 - SrcAlpha );
    FragColor.rgb =  Multiply + SrcContribution + DstContribution;
}
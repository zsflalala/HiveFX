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
    FragColor.a = Source.a + Destination.a * (1.0 - Source.a );
    const float Epsilon = 1e-6;
    if(FragColor.a > Epsilon)
        FragColor.rgb = ( Source.rgb * Source.a + Destination.rgb * Destination.a * ( 1.0 - Source.a ) ) / FragColor.a;
    else
        FragColor.rgb = vec3(0.0, 0.0, 0.0);
}
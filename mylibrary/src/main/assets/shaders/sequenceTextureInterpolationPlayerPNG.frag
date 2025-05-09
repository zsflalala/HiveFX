#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform float Factor;
uniform float Displacement;
uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;

void main()
{
    float CurrentTexOffset = TexCoord.x - Displacement * Factor;
    vec4 CurrentColor = texture(CurrentTexture, vec2(CurrentTexOffset, TexCoord.y));
    float NextTextureOffset = TexCoord.x + Displacement * (1.0 - Factor);
    vec4 NextColor = texture(NextTexture, vec2(NextTextureOffset, TexCoord.y));
    FragColor = mix(CurrentColor, NextColor, Factor);
}
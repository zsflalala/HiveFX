#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform float Factor;
uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;

void main()
{
    vec4 CurrentColor = texture(CurrentTexture, TexCoord);
    vec4 NextColor = texture(NextTexture, TexCoord);
    FragColor = mix(CurrentColor, NextColor, Factor);
    FragColor = CurrentColor;
}
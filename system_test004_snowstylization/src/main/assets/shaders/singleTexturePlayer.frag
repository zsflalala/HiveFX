#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D quadTexture;

void main()
{
    vec4 QuadColor = texture(quadTexture, TexCoord);
    FragColor = QuadColor;
}
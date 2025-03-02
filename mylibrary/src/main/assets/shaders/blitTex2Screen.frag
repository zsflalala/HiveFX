#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D fboTexture;

void main()
{
    FragColor = texture(fboTexture, TexCoord);
}
#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
uniform vec2 texUVOffset;
uniform vec2 texUVScale;
uniform sampler2D sequenceTexture;

void main()
{
    vec2 TexCoords = (TexCoord * texUVScale) + texUVOffset;
    vec4 TexColor = texture(sequenceTexture, TexCoords);
    FragColor = TexColor;
}
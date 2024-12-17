#version 300 es
precision mediump float;
out vec4 FragColor;

in      vec2 TexCoord;
uniform vec2 texUVOffset;
uniform vec2 texUVScale;
uniform sampler2D sequenceTexture;

void main()
{
    vec2 FinalUV = (TexCoord * texUVScale) + texUVOffset;
    vec4 SequenceColor = texture(sequenceTexture, FinalUV);
    FragColor = SequenceColor;
}
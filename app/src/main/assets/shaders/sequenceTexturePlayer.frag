#version 300 es
precision mediump float;
out vec4 FragColor;

in      vec2 TexCoord;
uniform vec2 uvOffset;
uniform vec2 uvScale;
uniform sampler2D sequenceTexture;

void main()
{
    vec2 FinalUV = (TexCoord * uvScale) + uvOffset;
    vec4 SequenceColor = texture(sequenceTexture, FinalUV);
    if(SequenceColor.a < 0.1)
        discard;
    FragColor = SequenceColor;
}
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
    vec3 TexColor = texture(sequenceTexture, TexCoords).rgb;

    float alpha = 1.0;
    float threshold = 0.2;
    if (length(TexColor) < threshold)
    {
        discard; // 直接丢弃片段
    }
    FragColor = vec4(TexColor.rgb + vec3(0.2, 0.2, 0.2), alpha);
}
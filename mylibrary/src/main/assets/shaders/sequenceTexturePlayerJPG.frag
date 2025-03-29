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

    float alpha = 1.0;
    float threshold = 0.5;
    vec3 keyColor = vec3(0, 0, 0);
    if (distance(TexColor.rgb, keyColor) < threshold)
    {
        discard;
    }
    FragColor = vec4(TexColor.rgb, alpha);
}
#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D indexTexture;
uniform int channelIndex;

void main()
{
    vec4 packedData = texture(indexTexture, TexCoord);
    uvec4 color = uvec4(packedData * 255.0 + 0.5);

    uint indices[8];
    indices[0] = (color.r >> 4u) & 0x0Fu; // R 高4位
    indices[1] = color.r & 0x0Fu;    // R 低4位
    indices[2] = (color.g >> 4u) & 0x0Fu; // G 高4位
    indices[3] = color.g & 0x0Fu;    // G 低4位
    indices[4] = (color.b >> 4u) & 0x0Fu; // B 高4位
    indices[5] = color.b & 0x0Fu;    // B 低4位
    indices[6] = (color.a >> 4u) & 0x0Fu; // A 高4位
    indices[7] = color.a & 0x0Fu;    // A 低4位

    float resultColor = float(indices[channelIndex]) / 15.0;

    if (resultColor < 1.0)
        discard;

    FragColor = vec4(resultColor, resultColor, resultColor, resultColor);
}
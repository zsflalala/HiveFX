#version 300 es
precision mediump float;

in vec2 outUV;
out vec4 FragColor;

uniform sampler2D Texture;
uniform vec2 _ScreenParams;
uniform vec2 _TextureParams;
uniform float _CoordBias;

vec2 GetBiasCoord(vec2 inQuadCoords, vec2 inTexParams)
{
    float widthRatio = _ScreenParams.y / inTexParams.y;
    vec2 tempTexCoords = vec2(inQuadCoords.x, inQuadCoords.y * widthRatio);
    tempTexCoords += vec2(0.0f, 1.0f / inTexParams.y * _CoordBias);
    return tempTexCoords;
}

void main()
{
    vec2 biasTexCoord = GetBiasCoord(outUV, _TextureParams);
    vec4 Color = vec4(texture(Texture, biasTexCoord).rgba);
    FragColor = Color;
}
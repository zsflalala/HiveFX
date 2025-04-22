#version 300 es
precision mediump float;

in vec2 outUV;
out vec4 FragColor;

uniform sampler2D Texture;
uniform vec2 _ScreenParams;
uniform vec2 _TextureParams;
uniform float _CoordBias;
uniform int   _Channel;

vec2 GetBiasCoord(vec2 inQuadCoords, vec2 inTexParams)
{
    float widthRatio = _ScreenParams.x / inTexParams.x;
    vec2 tempTexCoords = vec2(inQuadCoords.x * widthRatio, inQuadCoords.y);
    tempTexCoords += vec2(1.0f / inTexParams.x * _CoordBias, 0.0f);
    return tempTexCoords;
}

void main()
{
    vec2 biasTexCoord = GetBiasCoord(outUV, _TextureParams);
    float color;
    if (_Channel == 0)      color = texture(Texture, biasTexCoord).r;
    else if (_Channel == 1) color = texture(Texture, biasTexCoord).g;
    else if (_Channel == 2) color = texture(Texture, biasTexCoord).b;
    else if (_Channel == 3) color = texture(Texture, biasTexCoord).a;

    vec4 Color = vec4(color, color, color, 0.3);
    FragColor = Color;
}
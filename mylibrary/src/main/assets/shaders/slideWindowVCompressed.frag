#version 300 es
precision highp float;

in vec2 outUV;
out vec4 FragColor;

uniform sampler2D Texture;
uniform vec2 _ScreenParams;
uniform vec2 _TextureParams;
uniform float _CoordBias;
uniform int   _Channel;

vec2 GetBiasCoord(vec2 inQuadCoords, vec2 inTexParams)
{
    float heightRatio = _ScreenParams.y / inTexParams.y;
    vec2 tempTexCoords = vec2(
        inQuadCoords.x,
        inQuadCoords.y * heightRatio
    );

    tempTexCoords.y += _CoordBias / inTexParams.y;
    tempTexCoords.y = fract(tempTexCoords.y);

    return tempTexCoords;
}

void main()
{
    vec2 biasTexCoord = GetBiasCoord(outUV, _TextureParams);
    vec4 texColor = texture(Texture, biasTexCoord);
    float color = texColor[_Channel];
    if (color < 0.1)
        discard;
    color += 0.35;
    vec4 Color = vec4(color, color, color, 0.3);
    FragColor = Color;
}
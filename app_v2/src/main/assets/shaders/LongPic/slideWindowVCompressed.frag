#version 300 es
precision highp float;

in vec2 outUV;
out vec4 FragColor;

uniform sampler2D Texture;
uniform vec2 _ScreenParams;
uniform vec2 _TextureParams;
uniform float _CoordBias;
uniform int _Channel;

vec2 GetBiasCoord(vec2 inQuadCoords, vec2 inTexParams) {
    float aspectRatio = _ScreenParams.x / _ScreenParams.y;
    float textureAspect = inTexParams.x / inTexParams.y;

    float heightRatio = _ScreenParams.y / inTexParams.y;
    float adjustedRatio = heightRatio * max(1.0, aspectRatio);
    float yCoord = (inQuadCoords.y * adjustedRatio) + (_CoordBias / inTexParams.y);
    yCoord = mod(yCoord, 1.0);

    return vec2(inQuadCoords.x, yCoord);
}
void main()
{
    vec2 biasTexCoord = GetBiasCoord(outUV, _TextureParams);
    vec4 texColor = texture(Texture, biasTexCoord);
    float color = texColor[_Channel];
    if (color < 0.1)
        discard;

    color = clamp(color + 0.35, 0.0, 1.0);
    FragColor = vec4(color, color, color, 0.3);
}
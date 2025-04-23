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
     bool isLandscape = _ScreenParams.x > _ScreenParams.y;
     float targetRatio = isLandscape ? 0.3 : 0.9; // 横屏0.6，竖屏0.9 参数越大时雨滴越小
     float baseSize = max(_ScreenParams.x, _ScreenParams.y);
     float heightRatio = targetRatio * baseSize / inTexParams.y;

     float yCoord = (inQuadCoords.y * heightRatio) + (_CoordBias / inTexParams.y);
     return vec2(inQuadCoords.x, mod(yCoord, 1.0));
}
void main()
{
  vec2 biasTexCoord = GetBiasCoord(outUV, _TextureParams);
    vec4 texColor = texture(Texture, biasTexCoord);

    float color = texColor[_Channel];
    if (color < 0.1) {
            discard;
        }
    color = clamp(color * 1.5 + 0.5, 0.0, 1.0);

    FragColor = vec4(color, color, color, 0.5);
}
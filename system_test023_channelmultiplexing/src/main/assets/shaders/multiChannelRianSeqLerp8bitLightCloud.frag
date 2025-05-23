#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform float Factor;
uniform float Displacement;
uniform int CurrentChannel;
uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;

uniform vec2 uScreenSize;
uniform vec2 uTextureSize;
uniform float uScale;

void main()
{
      float YScale = (uTextureSize.y / uScreenSize.y) * uScale;
      if (TexCoord.y > YScale) {
          discard;
      }
      // 应用位移效果
      float CurrentFrameOffset = TexCoord.x - Displacement * Factor;
      vec4 CurrentTexColor = texture(CurrentTexture, vec2(CurrentFrameOffset, TexCoord.y / YScale));
      float CurrentColor = CurrentTexColor[CurrentChannel];

      int NextChannel = (CurrentChannel + 1) % 4;
      float NextFrameOffset = TexCoord.x + Displacement * (1.0 - Factor);
      vec4 NextTexColor = texture(NextTexture, vec2(NextFrameOffset, TexCoord.y / YScale));
      float NextColor = NextTexColor[NextChannel];

      float MixColor = mix(CurrentColor, NextColor, Factor);
      FragColor = vec4(1.0, 1.0, 1.0, MixColor);

}
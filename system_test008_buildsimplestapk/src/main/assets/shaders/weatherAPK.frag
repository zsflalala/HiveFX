#version 300 es
precision mediump float;

out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D backgroundTex;
uniform sampler2D middleTex;
uniform sampler2D foregroundTex;

void main()
{
    vec4 bg  = texture(backgroundTex, TexCoord);
    vec4 mid = texture(middleTex, TexCoord);
    vec4 fg  = texture(foregroundTex, TexCoord);

    // 混合顺序：背景 -> 中层 -> 前景
    vec4 result = mix(bg, mid, mid.a);
    result = mix(result, fg, fg.a);

    FragColor = result;
}
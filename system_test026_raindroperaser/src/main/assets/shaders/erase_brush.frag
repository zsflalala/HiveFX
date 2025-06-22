#version 300 es
precision mediump float;
out vec4 oFragColor;

in vec2 vTexCoord;

uniform vec2 uBrushCenter;
uniform float uBrushSize;

void main()
{
    float dist = distance(gl_FragCoord.xy, uBrushCenter);
    float alpha = 1.0 - step(dist, uBrushSize * 1.8); // 若 dist < 1.8 * size → alpha=1；否则=0
    if (alpha > 0.999) discard;
    oFragColor = vec4(alpha, 0.0, 0.0, 1.0); // 写入 R 通道
}
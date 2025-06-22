#version 300 es
precision mediump float;

uniform sampler2D uEraseMask;
uniform float uFadeStrength;

in vec2 vTexCoord;
out vec4 FragColor;

void main()
{
    float alpha = texture(uEraseMask, vTexCoord).r;
    alpha = min(alpha + uFadeStrength, 1.0); // 逐渐恢复为1
    FragColor = vec4(alpha, 0.0, 0.0, 1.0);  // 单通道R
}

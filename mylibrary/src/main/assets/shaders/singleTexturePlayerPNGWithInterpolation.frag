#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D quadTexture0;
uniform sampler2D quadTexture1;
uniform float interpolationFactor;

void main()
{
    vec4 color1 = texture(quadTexture0, TexCoord);
    vec4 color2 = texture(quadTexture1, TexCoord);

    // 使用 mix 进行插值
    FragColor = mix(color1, color2, interpolationFactor);
}
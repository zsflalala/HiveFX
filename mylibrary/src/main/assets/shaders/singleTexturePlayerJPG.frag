#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D quadTexture;

void main()
{
    vec3 QuadColor = texture(quadTexture, TexCoord).rgb;
    float alpha = 1.0;
    float threshold = 0.3;
    if (length(QuadColor) < threshold)
    {
        discard; // 直接丢弃片段
    }
    FragColor = vec4(QuadColor.rgb, alpha);
}
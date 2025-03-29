#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D quadTexture;

void main()
{
    vec4 QuadColor = texture(quadTexture, TexCoord);
    float alpha = 1.0;
    float threshold = 0.5;
    vec3 keyColor = vec3(0, 0, 0);
    if (distance(QuadColor.rgb, keyColor) < threshold)
    {
        discard;
    }
    FragColor = vec4(QuadColor.rgb, alpha);
}
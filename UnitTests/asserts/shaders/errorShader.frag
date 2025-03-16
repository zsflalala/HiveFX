#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
void main()
{
    FragColor = texture(fboTexture, TexCoord);
}
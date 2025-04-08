#version 320 es
precision lowp float;
out vec4 FragColor;
in vec2 vTexCoord;
uniform sampler2D texture1;
void main()
{
    FragColor=texture(texture1,vec2(vTexCoord.x,vTexCoord.y));
}
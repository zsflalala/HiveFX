#version 300 es
precision lowp float;
out vec4 FragColor;
in vec2 vTexCoord;
uniform sampler2D texture1;
void main()
{
   vec4 Color=texture(texture1,vec2(vTexCoord.x,vTexCoord.y));
    FragColor=vec4(Color.rgb,0.0);

}
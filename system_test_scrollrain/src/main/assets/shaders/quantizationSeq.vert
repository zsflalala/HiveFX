#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2  screenUVScale;
uniform vec2  screenUVOffset;
uniform float rotationAngle;

out vec2 TexCoord;

void main()
{
    mat2 scaleMatrix = mat2(
        screenUVScale.x, 0.0,
        0.0, screenUVScale.y
    );

    float cosTheta = cos(rotationAngle);
    float sinTheta = sin(rotationAngle);

    mat2 rotationMatrix = mat2(
        cosTheta, -sinTheta,
        sinTheta,  cosTheta
    );

    vec2 transformedPos = rotationMatrix * (scaleMatrix * aPos) + screenUVOffset;

    gl_Position = vec4(transformedPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
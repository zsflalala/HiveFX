#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2  screenUVScale;
uniform vec2  screenUVOffset;
uniform float rotationAngle;
uniform vec2  orthoBounds;

out vec2 TexCoord;

mat4 ortho2D(float left, float right, float bottom, float top)
{
    mat4 Result = mat4(1.0);
    Result[0][0] = 2.0 / (right - left);
    Result[1][1] = 2.0 / (top - bottom);
    Result[2][2] = -1.0;
    Result[3][0] = -(right + left) / (right - left);
    Result[3][1] = -(top + bottom) / (top - bottom);
    return Result;
}

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

    float orthoBoundsLeft  = -1.0;
    float orthoBoundsRight = 1.0;
    mat4 orthoMatrix;
    if (orthoBounds.x > orthoBounds.y)
    {
        // 横屏模式，宽度大于高度 // TODO: why need to multiply 2.0
        orthoMatrix = ortho2D(-orthoBounds.x / (2.0 * orthoBounds.y), orthoBounds.x / (2.0 * orthoBounds.y), orthoBoundsLeft, orthoBoundsRight);
    }
    else
    {
        // 竖屏模式，高度大于宽度
        orthoMatrix = ortho2D(orthoBoundsLeft, orthoBoundsRight, -orthoBounds.y / (2.0 * orthoBounds.x), orthoBounds.y / (2.0 * orthoBounds.x));
    }

    gl_Position = orthoMatrix * vec4(transformedPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
#pragma once
#include "OpenGLCommon.h"

namespace hiveVG
{
    class CScreenQuad
    {
    public:
        static CScreenQuad* getOrCreate();

        ~CScreenQuad();

        void bindAndDraw() const;

    private:
        CScreenQuad();

        GLuint m_VAOHandle;
        GLuint m_VertexBufferHandle;
        GLuint m_IndexBufferHandle;
    };
}

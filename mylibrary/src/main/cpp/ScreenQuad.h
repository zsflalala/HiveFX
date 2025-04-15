#pragma once
#include <mutex>
#include "OpenGLCommon.h"

namespace hiveVG
{
    class CScreenQuad
    {
    public:
        static CScreenQuad* getOrCreate();

        static void destroy();

        virtual ~CScreenQuad();

        virtual void bindAndDraw() const;

    private:

        static CScreenQuad* m_pQuad;
        static std::mutex m_Mutex;

        GLuint m_VAOHandle;
        GLuint m_VertexBufferHandle;
        GLuint m_IndexBufferHandle;
    protected:
        CScreenQuad();
    };
}

#pragma once
#include <mutex>
#include "OpenGLCommon.h"
#include "ScreenQuad.h"

namespace hiveVG
{
    class CBoard : public CScreenQuad
    {
    public:
        static CBoard* getOrCreate();

        static void destroy();

        ~CBoard() override;

        void bindAndDraw() const override;

    private:
        CBoard();

        static CBoard* m_pBoard;
        static std::mutex m_Mutex;

        GLuint m_VAOHandle;
        GLuint m_VertexBufferHandle;
        GLuint m_IndexBufferHandle;
    };
}

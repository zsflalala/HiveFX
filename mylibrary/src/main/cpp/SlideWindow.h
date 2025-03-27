#pragma once

#include <string>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;

    class CSlideWindow
    {
    public:
        CSlideWindow(const std::string& vTexturePath, float vSpeed, const std::string& vDirection, EPictureType::EPictureType vPictureType = EPictureType::PNG, bool vUseCompressed = false);
        virtual ~CSlideWindow();

        bool initTextureAndShaderProgram();
        void updateFrameAndDraw(int vWindowWidth, int vWindowHeight, double vDeltaTime, CScreenQuad* vQuad);

    private:
        bool            m_UseCompressed = false;
        int             m_TextureWidth;
        int             m_TextureHeight;
        EPictureType::EPictureType m_TextureType = EPictureType::PNG;
        float           m_CoordBias = 0.0f;
        float           m_SlideSpeed;
        std::string     m_TexturePath;
        std::string     m_SlideDirection;
        CShaderProgram* m_pShaderProgram = nullptr;
        CTexture2D*     m_pTexture       = nullptr;
    };
}

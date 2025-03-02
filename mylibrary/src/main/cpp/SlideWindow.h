#pragma once

#include <string>
#include <android/asset_manager.h>
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
        CSlideWindow(std::string& vTexturePath, float vSpeed, std::string& vDirection);
        virtual ~CSlideWindow();

        void createProgram(AAssetManager *vAssetManager);
        void loadTextures(AAssetManager *vAssetManager);
        void updateFrame(int vWindowWidth, int vWindowHeight, double vDeltaTime, CScreenQuad* vQuad);

    private:
        int          m_TextureWidth{};
        int          m_TextureHeight{};
        EPictureType::EPictureType m_TextureType = EPictureType::PNG;
        float        m_CoordBias = 0.0f;
        float        m_SlideSpeed;
        std::string  m_TexturePath;
        std::string  m_SlideDirection;
        CShaderProgram* m_pShaderProgram = nullptr;
        CTexture2D*     m_pTexture       = nullptr;
    };
}

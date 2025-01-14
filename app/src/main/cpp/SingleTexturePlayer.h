#pragma once

#include <string>
#include <vector>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;

    class CSingleTexturePlayer
    {
    public:
        CSingleTexturePlayer(const std::string& vTexturePath);
        ~CSingleTexturePlayer();

        bool initTextureAndShaderProgram(AAssetManager* vAssetManager);
        void updateFrame();

    private:
        std::string m_TexturePath;

        CTexture2D*	      m_pSingleTexture       = nullptr;
        CShaderProgram*   m_pSingleShaderProgram = nullptr;
    };
}
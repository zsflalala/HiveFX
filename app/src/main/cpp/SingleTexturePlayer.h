#pragma once

#include <string>
#include <vector>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include "Texture2D.h"
#include "ShaderProgram.h"

namespace hiveVG
{
    class CSingleTexturePlayer
    {
    public:
        CSingleTexturePlayer(const std::string& vTexturePath);
        ~CSingleTexturePlayer() = default;

        bool initTextureAndShaderProgram(AAssetManager* vAssetManager);
        void updateShaderAndTexture();

    private:
        std::string m_TexturePath;

        CTexture2D*	      m_pSingleTexture       = nullptr;
        CShaderProgram*   m_pSingleShaderProgram = nullptr;
    };
}


#pragma once

#include <string>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;

    class CSingleTexturePlayer
    {
    public:
        CSingleTexturePlayer(const std::string& vTexturePath, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        ~CSingleTexturePlayer();

        bool initTextureAndShaderProgram();
        void updateFrame();

    private:
        std::string m_TexturePath;

        CTexture2D*	      m_pSingleTexture       = nullptr;
        CShaderProgram*   m_pSingleShaderProgram = nullptr;
        EPictureType::EPictureType m_TextureType = EPictureType::PNG;
    };
}
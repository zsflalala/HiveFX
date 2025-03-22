#pragma once

#include <string>
#include <vector>

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;

    class CSingleTexturePlayer
    {
    public:
        CSingleTexturePlayer(const std::string& vTexturePath);
        ~CSingleTexturePlayer();

        bool initTextureAndShaderProgram();
        void updateFrame();

    private:
        std::string m_TexturePath;

        CTexture2D*	      m_pSingleTexture       = nullptr;
        CShaderProgram*   m_pSingleShaderProgram = nullptr;
    };
}
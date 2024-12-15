#pragma once

#include <string>
#include <vector>
#include <GLES3/gl3.h>

namespace hiveVG
{
    class CSingleTexturePlayer
    {
    public:
        CSingleTexturePlayer(const std::string& vTexturePath);
        ~CSingleTexturePlayer() = default;

    private:
        std::string m_TexturePath;

    };
}


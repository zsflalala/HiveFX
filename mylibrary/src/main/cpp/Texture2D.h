#pragma once
#include <string>
#include "Common.h"

#ifdef HIVE_ANDROID
    #include <android/asset_manager.h>
#endif

namespace hiveVG
{
    class CTexture2D
    {
    public:
        #ifdef HIVE_ANDROID
        static CTexture2D* loadTexture(AAssetManager *vAssetManager, const std::string &vTexturePath);
        static CTexture2D* loadTexture(AAssetManager *vAssetManager, const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType& vPictureType);
        static void        loadTextureFromCompressedPNG(AAssetManager *vAssetManager, const std::string &vTexturePath, int &voWidth, int &voHeight, std::vector<CTexture2D*>& vTexture2DVec);
        #endif
        static CTexture2D* loadTexture(const std::string &vTexturePath);
        static CTexture2D* loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType& vPictureType);
        static void        loadTextureFromCompressedPNG(const std::string &vTexturePath, int &voWidth, int &voHeight, std::vector<CTexture2D*>& vTexture2DVec);
        static CTexture2D* loadTextureFromMobile(const std::string &vTexturePath);
        static CTexture2D* createEmptyTexture(int vWidth, int vHeight, int vChannels);

        ~CTexture2D();

        [[nodiscard]] constexpr GLuint getTextureHandle() const { return m_TextureHandle; }

        void bindTexture() const;

    private:
        inline explicit CTexture2D(GLuint vTextureHandle);

        GLuint m_TextureHandle;
    };
}
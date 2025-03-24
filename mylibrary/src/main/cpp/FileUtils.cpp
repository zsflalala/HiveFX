#include "FileUtils.h"
#include "Common.h"
#include "platform.h"
#include "Logging.h"

using namespace hiveVG;

#ifdef HIVE_ANDROID

    #include <android/asset_manager.h>
    #include "AppContext.h"

    void *CFileUtils::openFile(const char *vPath)
    {
        auto* pAssetManager = static_cast<AAssetManager*>(CAppContext::getAssetManager());
        if (!pAssetManager)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "AssetManager is null.");
            return nullptr;
        }
        AAsset* pAsset = AAssetManager_open(pAssetManager, vPath, AASSET_MODE_BUFFER);
        if (!pAsset)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open asset: %s", vPath);
            return nullptr;
        }
        return pAsset;
    }

    size_t CFileUtils::getFileBytes(void* vFile)
    {
        if(!vFile)
            return 0;
        return AAsset_getLength(static_cast<AAsset*>(vFile));
    }

    void CFileUtils::closeFile(void* vFile)
    {
        AAsset_close(static_cast<AAsset*>(vFile));
    }

    std::unique_ptr<unsigned char[]> CFileUtils::readFromFile(const char* vPath, size_t& voAssetSize)
    {
        auto* pAssetManager = static_cast<AAssetManager*>(CAppContext::getAssetManager());
        if (!pAssetManager)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "AssetManager is null.");
            return nullptr;
        }

        AAsset* pAsset = AAssetManager_open(pAssetManager, vPath, AASSET_MODE_BUFFER);
        if (!pAsset)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open asset: %s", vPath);
            return nullptr;
        }

        voAssetSize = AAsset_getLength(pAsset);
        std::unique_ptr<unsigned char[]> pBuffer = std::make_unique<uint8_t[]>(voAssetSize);
        AAsset_read(pAsset, pBuffer.get(), voAssetSize);
        AAsset_close(pAsset);

        return pBuffer;
    }

#elif defined(HIVE_UNIT_TEST)

    #include <fstream>

    void *CFileUtils::openFile(const char *vPath)
    {
        FILE* pFile = nullptr;
        errno_t Flag = fopen_s(&pFile, vPath, "rb");
        if (Flag != 0)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open file: %s", vPath);
            return nullptr;
        }
        return pFile;
    }

    size_t CFileUtils::getFileBytes(void* vFile)
    {
        if (!vFile)
            return 0;
        FILE* pFile = static_cast<FILE*>(vFile);

        fseek(pFile, 0, SEEK_END);
        long Size = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);
        return static_cast<size_t>(Size);
    }

    void CFileUtils::closeFile(void* vFile)
    {
        if (vFile)
        {
            fclose(static_cast<FILE*>(vFile));
        }
    }

    std::unique_ptr<unsigned char[]> CFileUtils::readFromFile(const char* vPath, size_t& voAssetSize)
    {
        std::ifstream File(vPath, std::ios::binary | std::ios::ate);
        if (!File.is_open())
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open file: %s", vPath);
            return nullptr;
        }

        voAssetSize = File.tellg();
        File.seekg(0, std::ios::beg);

        auto pBuffer = std::make_unique<unsigned char[]>(voAssetSize);
        if (!File.read(reinterpret_cast<char*>(pBuffer.get()), voAssetSize))
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "Failed to read file: %s", vPath);
            return nullptr;
        }

        return pBuffer;
    }

#endif
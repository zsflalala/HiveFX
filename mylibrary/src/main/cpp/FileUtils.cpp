#include "FileUtils.h"
#include <fstream>
#include "Common.h"
#include "platform.h"
#include "Logging.h"

using namespace hiveVG;

#ifdef HIVE_ANDROID

    #include <android/asset_manager.h>
    #include "AppContext.h"

    void *CFileUtils::openFileByAssetManager(const char *vPath)
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

    size_t CFileUtils::getFileBytesByAsset(void* vFile)
    {
        if(!vFile)
            return 0;
        return AAsset_getLength(static_cast<AAsset*>(vFile));
    }

    void CFileUtils::closeAsset(void* vFile)
    {
        AAsset_close(static_cast<AAsset*>(vFile));
    }
#elif defined(HIVE_UNIT_TEST)
    void *CFileUtils::openFileByAssetManager(const char *vPath)
    {
        return nullptr;
    }

    size_t CFileUtils::getFileBytesByAsset(void* vFile)
    {
        return 0;
    }

    void CFileUtils::closeAsset(void* vFile)
    {

    }
#endif

    void *CFileUtils::openFile(const char *vPath)
    {
        FILE* pFile = fopen(vPath, "rb");
        if (!pFile) {
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
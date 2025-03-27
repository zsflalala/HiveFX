#include "FileUtils.h"
#include <fstream>
#include "Common.h"
#include "platform.h"
#include "Logging.h"

using namespace hiveVG;

#ifdef HIVE_ANDROID

    #include <android/asset_manager.h>
    #include "AppContext.h"

    void *CFileUtils::openFile(const char *vPath, bool vIsLoadFromAsset)
    {
        if(vIsLoadFromAsset)
        {
            auto* pAssetManager = static_cast<AAssetManager*>(CAppContext::getAssetManager());
            if (!pAssetManager)
            {
                LOG_WARN(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "AssetManager is null.");
                return nullptr;
            }
            AAsset* pAsset = AAssetManager_open(pAssetManager, vPath, AASSET_MODE_BUFFER);
            if (!pAsset)
            {
                LOG_WARN(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open asset: %s", vPath);
                return nullptr;
            }
            return pAsset;
        }
        else
        {
            FILE* pFile = fopen(vPath, "rb");
            if (!pFile)
            {
                LOG_WARN(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open file: %s", vPath);
                return nullptr;
            }
            return pFile;
        }
    }

    size_t CFileUtils::getFileBytes(void* vFile, bool vIsLoadFromAsset)
    {
        if(!vFile)
            return 0;
        if(vIsLoadFromAsset)
            return AAsset_getLength(static_cast<AAsset*>(vFile));
        else
        {
            FILE* pFile = static_cast<FILE*>(vFile);

            fseek(pFile, 0, SEEK_END);
            long Size = ftell(pFile);
            fseek(pFile, 0, SEEK_SET);
            return static_cast<size_t>(Size);
        }
    }

    void CFileUtils::closeFile(void* vFile, bool vIsLoadFromAsset)
    {
        if(!vFile)
            return;
        if(vIsLoadFromAsset)
            AAsset_close(static_cast<AAsset*>(vFile));
        else
            fclose(static_cast<FILE*>(vFile));

    }
#elif defined(HIVE_UNIT_TEST)
    void *CFileUtils::openFile(const char *vPath, bool vIsLoadFromAsset)
    {
        FILE* pFile;
        errno_t Error = fopen_s(&pFile, vPath, "rb");
        if (Error != 0)
        {
            LOG_WARN(hiveVG::TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open file: %s", vPath);
            return nullptr;
        }
        return pFile;
    }

    size_t CFileUtils::getFileBytes(void* vFile, bool vIsLoadFromAsset)
    {
        if (!vFile)
            return 0;
        FILE* pFile = static_cast<FILE*>(vFile);

        fseek(pFile, 0, SEEK_END);
        long Size = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);
        return static_cast<size_t>(Size);
    }

    void CFileUtils::closeFile(void* vFile, bool vIsLoadFromAsset)
    {
        if (vFile)
        {
            fclose(static_cast<FILE*>(vFile));
        }
    }
#endif
#pragma once
#include <cstdint>
#include <memory>
#include "Common.h"

namespace hiveVG
{
    class CFileUtils
    {
    public:
        static void*  openFileByAssetManager(const char* vPath);
        static size_t getFileBytesByAsset(void* vFile);
        static void   closeAsset(void* vFile);

        static void*  openFile(const char* vPath);
        static size_t getFileBytes(void* vFile);
        static void   closeFile(void* vFile);

        template <typename T>
        static int readFile(void* vFile, T* vBuffer, size_t vElementCount, bool vIsReadFromAsset = true);
    };

    template <typename T>
    int CFileUtils::readFile(void* vFile, T* vBuffer, size_t vElementCount, bool vIsReadFromAsset)
    {
        if (!vFile || !vBuffer)
        {
            LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Invalid parameters.");
            return -1;
        }
        size_t Flag;
        if(!vIsReadFromAsset)
        {
            Flag = fread(vBuffer, sizeof(T), vElementCount, static_cast<FILE*>(vFile));
            if (Flag != vElementCount)
                LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Error occurred while reading file.");
        }
        #ifdef HIVE_ANDROID
        else
        {
            Flag = AAsset_read(static_cast<AAsset*>(vFile), vBuffer, vElementCount * sizeof(T));
            if (static_cast<int>(Flag) < 0)
                LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Error occurred while reading file.");
        }
        #endif

        return static_cast<int>(Flag);
    }
}
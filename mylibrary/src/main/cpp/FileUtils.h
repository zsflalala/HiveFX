#pragma once
#include <cstdint>
#include <memory>
#include "Common.h"

namespace hiveVG
{
    class CFileUtils
    {
    public:
        static void*  openFile(const char* vPath, bool vIsLoadFromAsset = true);
        static size_t getFileBytes(void* vFile, bool vIsLoadFromAsset = true);
        static void   closeFile(void* vFile, bool vIsLoadFromAsset = true);

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

        #ifdef HIVE_ANDROID
        if (vIsReadFromAsset)
        {
            Flag = AAsset_read(static_cast<AAsset*>(vFile), vBuffer, vElementCount * sizeof(T));
            if (static_cast<int>(Flag) < 0)
                LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Error occurred while reading file.");
        }
        else
        {
            Flag = fread(vBuffer, sizeof(T), vElementCount, static_cast<FILE*>(vFile));
            if (Flag != vElementCount)
                LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Error occurred while reading file.");
        }

        #elif defined (HIVE_UNIT_TEST)

        Flag = fread(vBuffer, sizeof(T), vElementCount, static_cast<FILE*>(vFile));
        if (Flag != vElementCount)
            LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Error occurred while reading file.");

        #endif

        return static_cast<int>(Flag);
    }
}
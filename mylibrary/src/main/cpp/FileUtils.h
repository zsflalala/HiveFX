#pragma once
#include <cstdint>
#include <memory>
#include "Common.h"

namespace hiveVG
{
    class CFileUtils
    {
    public:
        static void* openFile(const char* vPath);
        static size_t getFileBytes(void* vFile);
        template <typename T>
        static size_t readFile(void* vFile, T* vBuffer, size_t vElementCount);
        static void closeFile(void* vFile);
        static std::unique_ptr<unsigned char[]>  readFromFile(const char* vPath, size_t& voAssetSize);
    };

    template <typename T>
    size_t CFileUtils::readFile(void* vFile, T* vBuffer, size_t vElementCount)
    {
        if (!vFile || !vBuffer)
        {
            LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Invalid parameters.");
            return -1;
        }
        #ifdef HIVE_ANDROID

            size_t Flag = AAsset_read(static_cast<AAsset*>(vFile), vBuffer, vElementCount * sizeof(T));
            if (Flag < 0)
                LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Error occurred while reading file.");

        #elif defined (HIVE_UNIT_TEST)

            size_t Flag = fread(vBuffer, sizeof(T), vElementCount, static_cast<FILE*>(vFile));
            if (Flag != vElementCount)
                LOG_ERROR(TAG_KEYWORD::FILE_UTILS_TAG, "Error occurred while reading file.");

        #endif
        return Flag;
    }
}
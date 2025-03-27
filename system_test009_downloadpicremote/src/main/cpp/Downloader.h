#pragma once
#include <jni.h>

namespace hiveVG
{
    class CDownloader
    {
    public:
        CDownloader() = delete;
        static void triggerDownload(JNIEnv* env);
    };
}


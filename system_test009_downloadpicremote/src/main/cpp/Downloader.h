#pragma once
#include <jni.h>
#include <string>

namespace hiveVG
{
    class CDownloader
    {
    public:
        CDownloader();
        ~CDownloader();

        bool downloadTexture(const std::string& vUrl, const std::string& vOutputFile);

    private:
        bool __initJNIEnv();
        bool __getJavaClass();
        bool __isFileExist(const std::string& vFile);

        bool      m_IsThreadAttachJVM = false;
        //TODO: 保存JNIEnv 是线程不安全的
        JNIEnv*   m_pJNIEnv = nullptr;
        jclass    m_JavaDownloader;
        jmethodID m_JavaDownloadMethod;
    };
}


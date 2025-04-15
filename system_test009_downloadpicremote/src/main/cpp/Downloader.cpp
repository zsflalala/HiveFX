#include "Downloader.h"
#include <thread>
#include <sys/stat.h>
#include "Common.h"
#include "jni_bridge.h"

using namespace hiveVG;

CDownloader::CDownloader()
{
    __initJNIEnv();
    __getJavaClass();
}

CDownloader::~CDownloader()
{
    if(!m_JavaDownloader)
        m_pJNIEnv->DeleteGlobalRef(m_JavaDownloader);

    if(!m_IsThreadAttachJVM)
    {
        assert(g_MainJvm);
        if(!g_MainJvm)
            return;
        g_MainJvm->DetachCurrentThread();
    }
    m_pJNIEnv = nullptr;
}

JNIEnv* GetValidJNIEnv() {
    JNIEnv* pEnv = nullptr;
    if (g_MainJvm->GetEnv((void**)&pEnv, JNI_VERSION_1_6) == JNI_EDETACHED)
    {
        g_MainJvm->AttachCurrentThread(&pEnv, nullptr);
    }
    return pEnv;
}

bool CDownloader::__initJNIEnv()
{
    assert(g_MainJvm);
    if(!g_MainJvm)
    {
        LOG_ERROR(TAG_KEYWORD::RENDERER_TAG, "Java virtual machine does not exist!");
        return false;
    }
    jint Status = g_MainJvm->GetEnv((void**)&m_pJNIEnv, JNI_VERSION_1_6);
    if(Status == JNI_OK)
        m_IsThreadAttachJVM = true;
    else
    {
        m_IsThreadAttachJVM = false;
        Status = g_MainJvm->AttachCurrentThread(&m_pJNIEnv, nullptr);
        if (Status != JNI_OK) {
            LOG_ERROR(TAG_KEYWORD::RENDERER_TAG, "Failed to attach thread!");
            m_pJNIEnv = nullptr;
            return false;
        }
    }
    return true;
}

bool CDownloader::downloadTexture(const std::string& vUrl, const std::string& vOutputFile)
{
    assert(m_pJNIEnv);
    assert(m_JavaDownloader);

    if(__isFileExist(vOutputFile))
        return true;

    jstring Url = m_pJNIEnv->NewStringUTF(vUrl.c_str());
    jstring OutputPath = m_pJNIEnv->NewStringUTF(vOutputFile.c_str());
    jboolean Result = m_pJNIEnv->CallStaticBooleanMethod(m_JavaDownloader, m_JavaDownloadMethod, Url, OutputPath);

    if (m_pJNIEnv->ExceptionCheck())
    {
        m_pJNIEnv->ExceptionDescribe();
        m_pJNIEnv->ExceptionClear();
    }

    m_pJNIEnv->DeleteLocalRef(Url);
    m_pJNIEnv->DeleteLocalRef(OutputPath);

    if (Result == 0)
    {
        LOG_ERROR(TAG_KEYWORD::RENDERER_TAG, "Download failed.");
        return false;
    }
    return true;
}

bool CDownloader::__getJavaClass()
{
    assert(m_pJNIEnv);
    auto JavaDownloader = loadAppClass(m_pJNIEnv, "com/hivefx/system_test009_downloadpicremote/Downloader");
    if (JavaDownloader == nullptr) {
        LOG_ERROR(TAG_KEYWORD::RENDERER_TAG, "Failed to find Downloader class!");
        return false;
    }
    m_JavaDownloader = (jclass) m_pJNIEnv->NewGlobalRef(JavaDownloader);
    m_pJNIEnv->DeleteLocalRef(JavaDownloader);
    m_JavaDownloadMethod = m_pJNIEnv->GetStaticMethodID(
            m_JavaDownloader,
            "downloadFile",
            "(Ljava/lang/String;Ljava/lang/String;)Z"
    );
    if (m_JavaDownloadMethod == nullptr)
    {
        LOG_ERROR(TAG_KEYWORD::RENDERER_TAG, "Failed to get method ID!");
        return false;
    }
    return false;
}

bool CDownloader::__isFileExist(const std::string& vFile)
{
    struct stat FileStat;
    return (stat(vFile.c_str(), &FileStat) == 0);
}

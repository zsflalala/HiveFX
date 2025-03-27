#include "Downloader.h"

using namespace hiveVG;
void CDownloader::triggerDownload(JNIEnv* env) {
    jclass JavaDownloader = env->FindClass("com/hivefx/system_test009_downloadpicremote/Downloader");
    jmethodID Method = env->GetStaticMethodID(
            JavaDownloader,
            "downloadFile",
            "(Ljava/lang/String;Ljava/lang/String;)V"
    );

    // 构造参数并调用
    jstring Url = env->NewStringUTF("https://example.com/file.zip");
    jstring OutputPath = env->NewStringUTF("textures/test.png");
    env->CallStaticVoidMethod(JavaDownloader, Method, Url, OutputPath);

    // 清理资源
    env->DeleteLocalRef(Url);
    env->DeleteLocalRef(OutputPath);
    env->DeleteLocalRef(JavaDownloader);
}

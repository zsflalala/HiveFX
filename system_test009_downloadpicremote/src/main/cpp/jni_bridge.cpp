#include <jni.h
#include <thread>
#include "Downloader.h"

JavaVM* globalJvm = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    globalJvm = vm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hivefx_system_1test009_1downloadpicremote_MainActivity_startNativeLogic(JNIEnv *env,
                                                                                 jobject thiz) {
    std::thread([] {
        // 附加当前线程到 JVM
        JNIEnv* env;
        globalJvm->AttachCurrentThread(&env, nullptr);

        // C++ 主动调用 Java 下载方法
        hiveVG::CDownloader::triggerDownload(env);
        globalJvm->DetachCurrentThread();
    }).detach();
}
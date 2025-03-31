#include "jni_bridge.h"
#include "Common.h"

JavaVM* g_MainJvm = nullptr;
jobject g_AppClassLoader = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    g_MainJvm = vm;
    JNIEnv* pEnv;
    if (vm->GetEnv((void**)&pEnv, JNI_VERSION_1_6) != JNI_OK)
    {
        return JNI_ERR;
    }

    jclass JavaThreadClass = pEnv->FindClass("java/lang/Thread");

    jmethodID CurrentThreadMethod = pEnv->GetStaticMethodID(JavaThreadClass, "currentThread", "()Ljava/lang/Thread;");
    jobject CurrentThread = pEnv->CallStaticObjectMethod(JavaThreadClass, CurrentThreadMethod);

    jmethodID GetContextClassLoaderMethod = pEnv->GetMethodID(JavaThreadClass, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
    jobject ClassLoader = pEnv->CallObjectMethod(CurrentThread, GetContextClassLoaderMethod);

    g_AppClassLoader = pEnv->NewGlobalRef(ClassLoader);

    pEnv->DeleteLocalRef(JavaThreadClass);
    pEnv->DeleteLocalRef(CurrentThread);
    pEnv->DeleteLocalRef(ClassLoader);

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (g_AppClassLoader)
    {
        env->DeleteGlobalRef(g_AppClassLoader);
        g_AppClassLoader = nullptr;
    }
}

namespace hiveVG
{
    jclass loadAppClass(JNIEnv* vEnv, const char* vClassName) {
        jclass ClassLoaderClass = vEnv->FindClass("java/lang/ClassLoader");
        jmethodID LoadClassMethod = vEnv->GetMethodID(ClassLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        jstring ClassNameStr = vEnv->NewStringUTF(vClassName);
        jclass TargetClass = (jclass)vEnv->CallObjectMethod(g_AppClassLoader, LoadClassMethod, ClassNameStr);
        vEnv->DeleteLocalRef(ClassNameStr);
        vEnv->DeleteLocalRef(ClassLoaderClass);
        return TargetClass;
    }
}
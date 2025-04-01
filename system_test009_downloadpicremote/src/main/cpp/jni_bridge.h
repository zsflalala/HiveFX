#pragma once
#include <jni.h>

extern JavaVM* g_MainJvm;
extern jobject g_AppClassLoader;

namespace hiveVG
{
    jclass loadAppClass(JNIEnv* vEnv, const char* vClassName);
}
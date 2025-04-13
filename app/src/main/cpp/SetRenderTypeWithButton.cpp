#include <jni.h>
#include "Renderer.h"

extern hiveVG::CRenderer* pSeqFrameRenderer;

extern "C"
JNIEXPORT void JNICALL
Java_com_hive_hivefx_MainActivity_nativeSetRenderType(JNIEnv *env, jobject thiz, jint render_type, jint enable_render_type, jboolean state)
{
    pSeqFrameRenderer->setRenderTypeWithButton(static_cast<hiveVG::ERenderType>(render_type), static_cast<hiveVG::ERenderType>(enable_render_type), state);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_hive_hivefx_MainActivity_nativeSetRenderType0(JNIEnv *env, jobject thiz, jint render_type, jboolean state)
{
    pSeqFrameRenderer->setRenderTypeWithButton(static_cast<hiveVG::ERenderType>(render_type), state);
}
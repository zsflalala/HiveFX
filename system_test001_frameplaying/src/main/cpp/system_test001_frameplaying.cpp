#include <jni.h>
#include <game-activity/GameActivity.cpp>
#include <game-activity/native_app_glue/android_native_app_glue.c>
#include <game-text-input/gametextinput.cpp>
#include "Renderer.h"
#include "Common.h"

extern "C"
{
void handleCmd(android_app* vApp, int32_t vCmd)
{
    switch (vCmd)
    {
        case APP_CMD_INIT_WINDOW:
            vApp->userData = new hiveVG::CRenderer(vApp);
            break;
        case APP_CMD_TERM_WINDOW:
            if (vApp->userData)
            {
                auto *pCSequenceFrameRenderer = reinterpret_cast<hiveVG::CRenderer*>(vApp->userData);
                vApp->userData = nullptr;
                delete pCSequenceFrameRenderer;
            }
            break;
        default:
            break;
    }
}

bool motion_event_filter_func(const GameActivityMotionEvent* vMotionEvent)
{
    auto sourceClass = vMotionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER || sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
}

void android_main(struct android_app* vApp)
{
    vApp->onAppCmd = handleCmd;
    android_app_set_motion_event_filter(vApp, motion_event_filter_func);

    do
    {
        bool Done = false;
        while (!Done)
        {
            int Timeout = 0;
            int Events;
            android_poll_source* pSource;
            int Result = ALooper_pollOnce(Timeout, nullptr, &Events,
                                          reinterpret_cast<void**>(&pSource));
            switch (Result)
            {
                case ALOOPER_POLL_TIMEOUT:
                    [[clang::fallthrough]];
                case ALOOPER_POLL_WAKE:
                    // No Events occurred before the Timeout or explicit wake. Stop checking for Events.
                    Done = true;
                    break;
                case ALOOPER_EVENT_ERROR:
                    LOG_ERROR(hiveVG::TAG_KEYWORD::MAIN_TAG, "ALooper_pollOnce returned an error");
                    break;
                case ALOOPER_POLL_CALLBACK:
                    break;
                default:
                    if (pSource)
                    {
                        pSource->process(vApp, pSource);
                    }
            }
        }

        if (vApp->userData)
        {
            auto *pSeqFrameRenderer = reinterpret_cast<hiveVG::CRenderer*>(vApp->userData);
            pSeqFrameRenderer->handleInput();
            pSeqFrameRenderer->renderScene();
        }
    } while (!vApp->destroyRequested);
}
}
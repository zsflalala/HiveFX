#include <jni.h>
#include <game-activity/GameActivity.cpp>
#include <game-activity/native_app_glue/android_native_app_glue.c>
#include <game-text-input/gametextinput.cpp>
#include "Renderer.h"
#include "Common.h"

extern "C"
{
    void handle_cmd(android_app *pApp, int32_t cmd)
    {
        switch (cmd)
        {
            case APP_CMD_INIT_WINDOW:
                pApp->userData = new hiveVG::CRenderer(pApp);
                break;
            case APP_CMD_TERM_WINDOW:
                if (pApp->userData)
                {
                    auto *pRenderer = reinterpret_cast<hiveVG::CRenderer *>(pApp->userData);
                    pApp->userData = nullptr;
                    delete pRenderer;
                }
                break;
            default:
                break;
        }
    }

    bool motion_event_filter_func(const GameActivityMotionEvent *motionEvent) {
        auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
        return (sourceClass == AINPUT_SOURCE_CLASS_POINTER || sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
    }

    void android_main(struct android_app *pApp) {
        pApp->onAppCmd = handle_cmd;
        android_app_set_motion_event_filter(pApp, motion_event_filter_func);

        do {
            bool done = false;
            while (!done)
            {
                int timeout = 0;
                int events;
                android_poll_source *pSource;
                int result = ALooper_pollOnce(timeout, nullptr, &events,
                                              reinterpret_cast<void**>(&pSource));
                switch (result)
                {
                    case ALOOPER_POLL_TIMEOUT:
                        [[clang::fallthrough]];
                    case ALOOPER_POLL_WAKE:
                        done = true;
                        break;
                    case ALOOPER_EVENT_ERROR:
                        LOG_ERROR(hiveVG::TAG_KEYWORD::MAIN_TAG, "ALooper_pollOnce returned an error");
                        break;
                    case ALOOPER_POLL_CALLBACK:
                        break;
                    default:
                        if (pSource)
                        {
                            pSource->process(pApp, pSource);
                        }
                }
            }

            if (pApp->userData)
            {
                auto *pRenderer = reinterpret_cast<hiveVG::CRenderer *>(pApp->userData);
                pRenderer->handleInput();
                pRenderer->render();
            }
        } while (!pApp->destroyRequested);
    }
}
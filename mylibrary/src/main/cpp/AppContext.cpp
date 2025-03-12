#include "AppContext.h"
#include <android/asset_manager.h>
#include <mutex>
#include "Common.h"

namespace hiveVG
{
    static std::mutex g_AppMutex;
    static AAssetManager* g_pAssetManager = nullptr;

    void setAssetManager(void *vAssetManager)
    {
        std::lock_guard<std::mutex> lock(g_AppMutex);
        g_pAssetManager = static_cast<AAssetManager*>(vAssetManager);
    }

    void* getAssetManager()
    {
        std::lock_guard<std::mutex> lock(g_AppMutex);
        if (!g_pAssetManager)
            LOG_ERROR(TAG_KEYWORD::APP_CONTEXT_TAG, "Asset manager does not exist.");
        return g_pAssetManager;
    }
}
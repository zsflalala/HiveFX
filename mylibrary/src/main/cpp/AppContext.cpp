#include "AppContext.h"
#include <android/asset_manager.h>
#include "Common.h"

namespace hiveVG
{
    std::mutex CAppContext::m_AppMutex;
    AAssetManager* CAppContext::m_pAssetManager = nullptr;
    std::string CAppContext::m_StoragePath = "";

    void CAppContext::setAssetManager(void *vAssetManager)
    {
        std::lock_guard<std::mutex> Lock(m_AppMutex);
        m_pAssetManager = static_cast<AAssetManager*>(vAssetManager);
    }

    void* CAppContext::getAssetManager()
    {
        std::lock_guard<std::mutex> Lock(m_AppMutex);
        if (!m_pAssetManager)
            LOG_ERROR(TAG_KEYWORD::APP_CONTEXT_TAG, "Asset manager does not exist.");
        return m_pAssetManager;
    }

    void CAppContext::setStoragePath(const std::string &vPath)
    {
        m_StoragePath = vPath;
    }

    std::string CAppContext::getStoragePath()
    {
        if(m_StoragePath.empty())
            LOG_ERROR(TAG_KEYWORD::APP_CONTEXT_TAG, "Storage path does not set.");
        return m_StoragePath;
    }
}
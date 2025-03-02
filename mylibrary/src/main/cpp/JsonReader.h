#pragma once

#include <android/asset_manager.h>
#include <string>
#include <json/json.h>

namespace hiveVG
{
    class CJsonReader
    {
    public:
        CJsonReader(AAssetManager *vAssetManager, const std::string& vFilePath);

        bool        hasKey(const std::string& vKey)     const;
        std::string getString(const std::string& vKey)  const;
        int         getInt(const std::string& vKey)     const;
        double      getDouble(const std::string& vKey)  const;
        bool        getBool(const std::string& vKey)    const;
        Json::Value getArray(const std::string& vKey)   const;
        Json::Value getObject(const std::string& vKey)  const;

    private:
        void __checkKeyExists(const std::string& vKey) const;

        Json::Value m_Root;
    };
}
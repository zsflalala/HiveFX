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

        [[nodiscard]] bool        hasKey(const std::string& vKey)     const;
        [[nodiscard]] std::string getString(const std::string& vKey)  const;
        [[nodiscard]] int         getInt(const std::string& vKey)     const;
        [[nodiscard]] double      getDouble(const std::string& vKey)  const;
        [[nodiscard]] bool        getBool(const std::string& vKey)    const;
        [[nodiscard]] Json::Value getArray(const std::string& vKey)   const;
        [[nodiscard]] Json::Value getObject(const std::string& vKey)  const;

    private:
        void __checkKeyExists(const std::string& vKey) const;

        Json::Value m_Root;
    };
}
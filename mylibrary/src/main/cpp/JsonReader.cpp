#include "JsonReader.h"
#include "Common.h"

using namespace hiveVG;

CJsonReader::CJsonReader(AAssetManager *vAssetManager, const std::string& vFilePath)
{
    AAsset* pAsset = AAssetManager_open(vAssetManager, vFilePath.c_str(), AASSET_MODE_BUFFER);
    if (!pAsset)
    {
        throw std::runtime_error("Failed to open asset file: " + vFilePath);
    }

    const char* pData = static_cast<const char*>(AAsset_getBuffer(pAsset));
    size_t Size = AAsset_getLength(pAsset);
    if (!pData || Size == 0)
    {
        AAsset_close(pAsset);
        throw std::runtime_error("Failed to read asset file: " + vFilePath);
    }

    std::string JsonContent(pData, Size);
    AAsset_close(pAsset);

    Json::CharReaderBuilder ReaderBuilder;
    std::unique_ptr<Json::CharReader> Reader(ReaderBuilder.newCharReader());
    std::string Errors;
    if (!Reader->parse(JsonContent.c_str(), JsonContent.c_str() + JsonContent.size(), &m_Root, &Errors))
    {
        throw std::runtime_error("JSON parse error: " + Errors);
    }
}

bool CJsonReader::hasKey(const std::string& vKey) const
{
    return m_Root.isMember(vKey);
}

std::string CJsonReader::getString(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey].asString();
}

int CJsonReader::getInt(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey].asInt();
}

double CJsonReader::getDouble(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey].asDouble();
}

bool CJsonReader::getBool(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey].asBool();
}

Json::Value CJsonReader::getArray(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey];
}

Json::Value CJsonReader::getObject(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey];
}

void CJsonReader::__checkKeyExists(const std::string &vKey) const
{
    if (!hasKey(vKey)) throw std::runtime_error("Key not found: " + vKey);
}
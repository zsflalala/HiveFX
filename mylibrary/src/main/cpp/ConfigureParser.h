#pragma once
#include <string>

namespace hiveVG
{
    class CJsonReader;
    class CSequenceFramePlayer;

    class CConfigureParser
    {
    public:
        static CSequenceFramePlayer* createSequenceFramePlayer(const CJsonReader* vJsonReader);
        static CSequenceFramePlayer* createSequencePlayerForHuawei(const CJsonReader *vJsonReader, std::string& voVert, std::string& voFrag);

    private:
        CConfigureParser() {}
    };
}
#pragma once
#include<string>
//#include <cstdint>

namespace hiveVG
{

    class CEffectTester
    {
    public:
        CEffectTester();
        ~CEffectTester();
        void init(int vWidth, int vHeight);
        bool setResolution(int vWidth, int vHeight);
        bool setFilePath(const std::string& vFilePath);
        bool saveScreenBuffer(const std::string& vTexName);
        void evaluateImagesQuality(const std::string& vTexPath1, const std::string& vTexPath2, double& voPSNR, double& voSSIM, const char* vTexPath = nullptr);

        double computePSNR(const unsigned char* vImg1, const unsigned char* vImg2, int vSize, int vStride);
        double computeSSIM(const unsigned char* vImg1, const unsigned char* vImg2, int vWidth, int vHeight, int vStride);

    private:
        void __flipBuffer();
        unsigned char* __loadImage(const std::string& vFilePath, int& voWidth, int& voHeight, int& voChannels);
        double __computeMean(const unsigned char* vImg, int vPixelSize, int vStride, int vChannel);
        double __computeVariance(const unsigned char* vImg, double vMean, int vPixelSize, int vStride, int vChannel);
        std::vector<double> __computeVarAndCovar(const unsigned char* vImg1, const unsigned char* vImg2, double vMean1, double vMean2, int vPixelSize, int vStride, int vChannel);
        double __computeMean(const std::vector<double>& vVector);

        int m_Width;
        int m_Height;

        unsigned char* m_pBuffer = nullptr;
        std::string m_FilePath;
    };

}

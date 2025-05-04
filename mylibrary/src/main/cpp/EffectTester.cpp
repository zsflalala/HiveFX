#include "EffectTester.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "stb_image.h"
#include "Common.h"
#include "AppContext.h"

using namespace hiveVG;

CEffectTester::CEffectTester()
{
    std::string StoragePath = CAppContext::getStoragePath();
    if(!StoragePath.empty())
        setFilePath(StoragePath);
}

CEffectTester::~CEffectTester()
{
    if(m_pBuffer)
        delete[] m_pBuffer;
}

void CEffectTester::init(int vWidth, int vHeight)
{
    setResolution(vWidth, vHeight);
}

bool CEffectTester::setResolution(int vWidth, int vHeight)
{
    if(vWidth <= 0 || vHeight <= 0)
    {
        LOG_ERROR(TAG_KEYWORD::EFFECT_TEST_TAG, "Invalid resolution.");
        return false;
    }
    m_Width = vWidth;
    m_Height = vHeight;
    return true;
}

bool CEffectTester::saveScreenBuffer(const std::string& vTexName)
{
    size_t BufferSize = m_Width * m_Height * 4;
    if(m_pBuffer)
        delete[] m_pBuffer;
    m_pBuffer = new unsigned char[BufferSize];
    glReadPixels(0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, m_pBuffer);
    __flipBuffer();
    std::string OutputTex = m_FilePath + vTexName;
    int IsSaveSucceed = stbi_write_png(OutputTex.c_str(), m_Width, m_Height, 4, m_pBuffer, m_Width * 4);
    delete[] m_pBuffer;
    m_pBuffer = nullptr;
    if (!IsSaveSucceed)
    {
        LOG_ERROR(TAG_KEYWORD::EFFECT_TEST_TAG, "Saving texture failed.");
        return false;
    }
    LOG_INFO(TAG_KEYWORD::EFFECT_TEST_TAG, "Saving texture %s succeed.", vTexName.c_str());
    return true;
}

bool CEffectTester::setFilePath(const std::string &vFilePath)
{
    char EndChar = vFilePath.back();
    std::string FilePath;
    // TODO: 添加路径合法性校验
    if(EndChar != '\\' && EndChar != '/')
        FilePath = vFilePath + "/";
    m_FilePath = FilePath;
    return true;
}

void CEffectTester::evaluateImagesQuality(const std::string &vTexName1, const std::string &vTexName2, double& voPSNR, double& voSSIM, const char* vTexPath)
{
    std::string ImgPath;
    if(vTexPath)
        ImgPath = vTexPath;
    else
        ImgPath = m_FilePath;
    int Img1Width, Img1Height, Img1Channels, Img2Width, Img2Height, Img2Channels;
    unsigned char* pImg1 = __loadImage(ImgPath + vTexName1, Img1Width, Img1Height, Img1Channels);
    unsigned char* pImg2 = __loadImage(ImgPath + vTexName2, Img2Width, Img2Height, Img2Channels);

    if(!pImg1 || !pImg2)
    {
        LOG_ERROR(TAG_KEYWORD::EFFECT_TEST_TAG, "Failed to load Image \"%s\" or \"%s\".", vTexName1.c_str(), vTexName2.c_str());
        return;
    }

    bool IsSizeEqual = Img1Width == Img2Width && Img1Height==Img2Height && Img1Channels==Img2Channels;
    assert(IsSizeEqual);
    if(!IsSizeEqual)
    {
        LOG_ERROR(TAG_KEYWORD::EFFECT_TEST_TAG, "Sizes of image \"%s\" and \"%s\" are different.", vTexName1.c_str(), vTexName2.c_str());
        return;
    }

    voPSNR = computePSNR(pImg1, pImg2, Img1Width * Img1Height, 3);
    voSSIM = computeSSIM(pImg1, pImg2, Img1Width, Img1Height, 3);

    stbi_image_free(pImg1);
    stbi_image_free(pImg2);
}

double CEffectTester::computePSNR(const unsigned char* vImg1, const unsigned char* vImg2, int vSize, int vStride)
{
    double* MSE = new double[vStride];
    memset(MSE, 0, vStride * sizeof(double));
    for (int i = 0; i < vSize; i++)
    {
        for(int j = 0; j < vStride; j++)
        {
            int Diff = static_cast<int>(vImg1[i * vStride + j]) - static_cast<int>(vImg2[i * vStride + j]);
            MSE[j] += Diff * Diff;
        }
    }
    double MSECount = 0.0;
    for(int j = 0; j < vStride; j++)
    {
        MSECount += MSE[j]/vSize;
    }
    delete[] MSE;
    MSECount /= vStride;
    return (MSECount == 0) ? INFINITY : 10.0 * log10(255.0 * 255.0 / MSECount);
}

double CEffectTester::computeSSIM(const unsigned char* vImg1, const unsigned char* vImg2, int vWidth, int vHeight, int vStride)
{
    const double maxPixel = 255.0;
    const double C1 = (0.01 * maxPixel) * (0.01 * maxPixel);
    const double C2 = (0.03 * maxPixel) * (0.03 * maxPixel);

    int PixelCounts = vWidth * vHeight;
    std::vector<double> SSIMOfAllChannel;
    for(int i = 0; i<vStride; i++)
    {
        auto Mean1 = __computeMean(vImg1, PixelCounts, vStride, i);
        auto Mean2 = __computeMean(vImg2, PixelCounts, vStride, i);
        auto Var1 = __computeVariance(vImg1, Mean1, PixelCounts, vStride, i);
        auto Var2 = __computeVariance(vImg2, Mean2, PixelCounts, vStride, i);
        std::vector<double> Vars = __computeVarAndCovar(vImg1, vImg2, Mean1, Mean2, PixelCounts, vStride, i);
        double Covar = Vars[2];
        double Numerator = (2 * Mean1 * Mean2 + C1) * (2 * Covar + C2);
        double Denominator = (Mean1 * Mean1 + Mean2 * Mean2 + C1) * (Var1 + Var2 + C2);
        SSIMOfAllChannel.push_back(Numerator / Denominator);
    }
    return __computeMean(SSIMOfAllChannel);
}

void CEffectTester::__flipBuffer()
{
    assert(m_pBuffer);
    size_t BufferSize = m_Width * m_Height * 4;
    unsigned char* pFlippedBuffer = new unsigned char[BufferSize];
    for (int row = 0; row < m_Height; row++) {
        memcpy(
                pFlippedBuffer + row * m_Width * 4,
                m_pBuffer + (m_Height - row - 1) * m_Width * 4,
                m_Width * 4
        );
    }
    delete[] m_pBuffer;
    m_pBuffer = pFlippedBuffer;
}

double CEffectTester::__computeMean(const unsigned char* vImg, int vPixelSize, int vStride, int vChannel)
{
    double Sum = 0.0;
    for (int i = 0; i < vPixelSize; i++)
    {
        Sum += vImg[i * vStride + vChannel];
    }
    return Sum/vPixelSize;
}

double CEffectTester::__computeVariance(const unsigned char* vImg, double vMean, int vPixelSize, int vStride, int vChannel)
{
    double Variance = 0.0;
    for (int i = 0; i < vPixelSize; i++)
    {
        double Diff = vImg[i * vStride + vChannel] - vMean;
        Variance += Diff * Diff;
    }
    return Variance/vPixelSize;
}

std::vector<double> CEffectTester::__computeVarAndCovar(const unsigned char* vImg1, const unsigned char* vImg2, double vMean1, double vMean2, int vPixelSize, int vStride, int vChannel)
{
    double Variance1 = 0.0, Variance2 = 0.0, Covariance = 0.0;
    double Diff1, Diff2;
    for (int i = 0; i < vPixelSize; i++)
    {
        Diff1 = vImg1[i * vStride + vChannel] - vMean1;
        Diff2 = vImg2[i * vStride + vChannel] - vMean2;

        Variance1 += Diff1 * Diff1;
        Variance2 += Diff2 * Diff2;
        Covariance += Diff1 * Diff2;
    }

    std::vector<double> Returns(3);
    Returns[0] = Variance1 / vPixelSize;
    Returns[1] = Variance2 / vPixelSize;
    Returns[2] = Covariance / vPixelSize;
    return Returns;
}

unsigned char* CEffectTester::__loadImage(const std::string &vFilePath, int& voWidth, int& voHeight, int& voChannels)
{
    unsigned char* pPixels = stbi_load(vFilePath.c_str(), &voWidth, &voHeight, &voChannels, 3);

    if (pPixels)
    {
        LOG_INFO(TAG_KEYWORD::EFFECT_TEST_TAG, "读入图像 %s , 图像尺寸: %d x %d, 通道数: %d", vFilePath.c_str(), voWidth, voHeight, voChannels);
        return pPixels;
    } else {
        LOG_ERROR(TAG_KEYWORD::EFFECT_TEST_TAG, "加载图像 %s 失败: %s", vFilePath.c_str(), stbi_failure_reason());
        return nullptr;
    }
}

double CEffectTester::__computeMean(const std::vector<double>& vVector)
{
    double Sum = 0;
    for(int i = 0; i< vVector.size(); i++)
    {
        Sum += vVector[i];
    }
    return Sum/vVector.size();
}

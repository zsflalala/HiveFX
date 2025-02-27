#include "SnowStylizer.h"

using namespace hiveVG;

#include <filesystem>
#include <random>
#include <cassert>
#include "Common.h"

namespace fs = std::filesystem;

const float CSnowStylizer::m_Pai = 3.1415f;

CSnowStylizer::CSnowStylizer()
        : m_SnowHighThreshold(3.0 / 4), m_SnowMaxHighProportion(1.0 / 40), m_SnowShapeFreq(13), m_SnowShapeAmplitude(3), m_ShapeFreqMultiplier(9.0 / 10), m_ShapeAmplitudeMultiplier(9.0 / 10)
{ }

bool CSnowStylizer::loadImg(AAssetManager* vAssetManager, const std::string& vImgPath)
{
    AAsset* pAsset = AAssetManager_open(vAssetManager, vImgPath.c_str(), AASSET_MODE_BUFFER);
    if (!pAsset)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SYSTEMTEST_TAG, "Asset initialization falied.");
        return false;
    }
    size_t FileSize = AAsset_getLength(pAsset);
    std::vector<uchar> Buffer(FileSize);
    AAsset_read(pAsset, Buffer.data(), FileSize);
    AAsset_close(pAsset);

    m_OriginImage = cv::imdecode(Buffer, cv::IMREAD_UNCHANGED);
    if (m_OriginImage.empty())
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"Could not open or find the image [%s].", vImgPath.c_str());
        return false;
    }
    assert(m_OriginImage.channels() == 4);
    if (m_OriginImage.channels() != 4)
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"Alpha channel does not exist.");
        m_OriginImage.release();
        return false;
    }
    m_SnowImage = cv::Mat(m_OriginImage.size(), CV_8U, cv::Scalar(0));
    __loadFileSavePath(vImgPath);
    m_LayerSnowImgs.push_back(m_OriginImage);
    return true;
}

bool CSnowStylizer::loadImg(AAssetManager* vAssetManager, const std::string& vImgPath, const cv::Vec3b& BackgroundColor)
{
    AAsset* pAsset = AAssetManager_open(vAssetManager, vImgPath.c_str(), AASSET_MODE_BUFFER);
    if (!pAsset)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SYSTEMTEST_TAG, "Asset initialization falied.");
        return false;
    }
    size_t FileSize = AAsset_getLength(pAsset);
    std::vector<uchar> Buffer(FileSize);
    AAsset_read(pAsset, Buffer.data(), FileSize);
    AAsset_close(pAsset);

    m_OriginImage = cv::imdecode(Buffer, cv::IMREAD_UNCHANGED);
    if (m_OriginImage.empty())
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"Could not open or find the image.");
        return false;
    }
    cv::Mat AlphaChannel = cv::Mat(m_OriginImage.size(), CV_8UC1);
    for (int y = 0; y < m_OriginImage.rows; ++y)
    {
        for (int x = 0; x < m_OriginImage.cols; ++x)
        {
            cv::Vec3b PixelValue = m_OriginImage.at<cv::Vec3b>(y, x);
            if (PixelValue == BackgroundColor)
            {
                AlphaChannel.at<uchar>(y, x) = 0;
            }
            else
            {
                AlphaChannel.at<uchar>(y, x) = 255;
            }
        }
    }

    std::vector<cv::Mat> BGRChannels;
    cv::split(m_OriginImage, BGRChannels);
    BGRChannels.push_back(AlphaChannel);
    cv::merge(BGRChannels, m_OriginImage);

    m_SnowImage = cv::Mat(m_OriginImage.size(), CV_8U, cv::Scalar(0));
    __loadFileSavePath(vImgPath);
    m_LayerSnowImgs.push_back(m_OriginImage);
    return true;
}

bool CSnowStylizer::setShapeFreq(float vFrequency)
{
    if (vFrequency > 0)
    {
        m_SnowShapeFreq = vFrequency;
        return true;
    }
    else
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"Snow Shape frequency illegal.");
        return false;
    }
}

bool CSnowStylizer::setSnowHighProportion(float vProportion)
{
    if (vProportion > 0)
    {
        m_SnowMaxHighProportion = vProportion;
        return true;
    }
    else
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"Snow height proportion illegal.");
        return false;
    }
}

bool CSnowStylizer::setShapeAmplitude(float vAmplitude)
{
    if (vAmplitude > 0)
    {
        m_SnowShapeAmplitude = vAmplitude;
        return true;
    }
    else
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"Snow shape amplitude illegal.");
        return false;
    }
}

bool CSnowStylizer::setOutputPath(const std::string& vOutputPath)
{
    if (fs::exists(fs::path(vOutputPath)))
    {
        m_FileNameFormat._FilePath = vOutputPath;
        return true;
    }
    else
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"File path does not exist.");
    return false;
}

bool CSnowStylizer::setOutputName(const std::string& vOutputName)
{
    std::string illegalChars = "\\/*?\"<>|:";
    for (char c : vOutputName)
    {
        if (illegalChars.find(c) != std::string::npos)
        {
            LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"File name contains illegal characters.");
            return false;
        }
    }

    m_FileNameFormat._FileName = vOutputName + ".png";
    return true;
}

bool CSnowStylizer::generateSnow(int vSnowLayerNum)
{
    assert(!m_OriginImage.empty());
    if (m_OriginImage.empty())
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"No image loaded.");
        return false;
    }

    cv::Mat BinaryImg;
    __binarizeImg(BinaryImg);

    std::vector<SContourGradInfo> SurfaceNormals;
    __calculatNormal(SurfaceNormals, BinaryImg);

    int SnowMaxHigh = static_cast<int>(m_OriginImage.rows * m_SnowMaxHighProportion);

    for (int i = 1; i <= vSnowLayerNum; i++)
    {
        int MaxHigh = static_cast<int>(SnowMaxHigh * ((float)i / vSnowLayerNum));
        float ShapePhase = __getRandomFloat(0.0f, 2 * m_Pai);
        __generateLayerSnow(MaxHigh, ShapePhase, SurfaceNormals);
        m_SnowShapeFreq *= m_ShapeFreqMultiplier;
        m_SnowShapeAmplitude *= m_ShapeAmplitudeMultiplier;
    }

    __saveImg();
    return true;
}

void CSnowStylizer::__generateLayerSnow(int vMaxHigh, float vShapePhase, const std::vector<SContourGradInfo>& vNormalList)
{
    for (auto& NormalInfo : vNormalList)
    {
        int SnowHigh = vMaxHigh * NormalInfo.GradY;
        float TexCoordX = static_cast<float>(NormalInfo.CoordX) / m_OriginImage.cols;
        int SnowHighNoise = std::sin(TexCoordX * 2 * m_Pai * m_SnowShapeFreq + vShapePhase) * m_SnowShapeAmplitude;
        int SnowNoiseFBM = SnowHighNoise + std::sin(TexCoordX * 2 * m_SnowShapeFreq) * m_SnowShapeAmplitude * 2 / 3;
        SnowHigh = std::min(SnowHigh, int(vMaxHigh * m_SnowHighThreshold)) + SnowNoiseFBM;
        for (int i = 0; i < SnowHigh; i++)
        {
            int ActualCoordY = NormalInfo.CoordY - i;
            if (ActualCoordY >= 0)
            {
                if (m_OriginImage.at<cv::Vec4b>(ActualCoordY, NormalInfo.CoordX)[3] < 200)
                    m_SnowImage.at<uchar>(ActualCoordY, NormalInfo.CoordX) = UCHAR_MAX;
            }
            else
                break;
        }
    }

    cv::Mat BlurredImg;
    cv::blur(m_SnowImage, BlurredImg, cv::Size(5, 5));

    cv::Mat OriginImgAlpha;
    cv::extractChannel(m_OriginImage, OriginImgAlpha, 3);
    cv::Mat SnowImg = cv::Mat(m_OriginImage.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0));

    for (int i = 0; i < m_OriginImage.rows; ++i)
    {
        for (int j = 0; j < m_OriginImage.cols; ++j)
        {
            float OriginAlphaValue = OriginImgAlpha.at<uchar>(i, j) / static_cast<float>(UCHAR_MAX);
            float SnowAlpha = BlurredImg.at<uchar>(i, j) / static_cast<float>(UCHAR_MAX);
            if (OriginAlphaValue > 0.0f || SnowAlpha > 0)
            {
                float Alpha = SnowAlpha + OriginAlphaValue * (1 - SnowAlpha);
                for (int c = 0; c < 3; c++)
                {
                    SnowImg.at<cv::Vec4b>(i, j)[c] = static_cast<uchar>(UCHAR_MAX * SnowAlpha + (1 - SnowAlpha) * OriginAlphaValue * m_OriginImage.at<cv::Vec4b>(i, j)[c]) / Alpha;
                }
                SnowImg.at<cv::Vec4b>(i, j)[3] = Alpha * UCHAR_MAX;
            }
        }
    }

    m_LayerSnowImgs.push_back(SnowImg);
}

float CSnowStylizer::__getRandomFloat(float vMin, float vMax)
{
    unsigned Seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 RandomGenerator(Seed);
    std::uniform_real_distribution<> RandomDistribution(vMin, vMax);
    return static_cast<float>(RandomDistribution(RandomGenerator));
}

int CSnowStylizer::__getRandomInt(int vSeed, int vMin, int vMax)
{
    std::mt19937 RandomGenerator(vSeed);
    std::uniform_int_distribution<> RandomDistribution(vMin, vMax);
    return RandomDistribution(RandomGenerator);
}

void CSnowStylizer::__loadFileSavePath(const std::string& vFilePath)
{
    fs::path PathObj(vFilePath);
    m_FileNameFormat._FilePath = PathObj.parent_path().string();
    m_FileNameFormat._FileName = __formatFileName(PathObj.filename().string());
    assert(!m_FileNameFormat._FileName.empty());
}

void CSnowStylizer::__saveImg()
{
    for (int i = 0; i < m_LayerSnowImgs.size(); i++)
    {
        std::string FileName = std::vformat(m_FileNameFormat._FileName, std::make_format_args(i));
        std::string OutputFile;
        OutputFile = hiveVG::P60SaveToPhotoPath + m_FileNameFormat._FilePath + "/" + FileName;
        bool SuccessFlag = cv::imwrite(OutputFile, m_LayerSnowImgs[i]);
        if (SuccessFlag)
        {
            LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"Image saved successfully as %s", FileName.c_str());
        }
        else
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG, "Failed to save the image.");
        }
    }
}

void CSnowStylizer::__showImg(const cv::Mat& vImg, const std::string& vWindowName)
{
    if (vImg.empty())
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG,"Could not open or find the image");
        return;
    }

    cv::Mat ShowImg;

    if (vImg.channels() == 2)
    {
        std::vector<cv::Mat> Channels;
        cv::split(vImg, Channels);

        cv::Mat ZeroChannel = cv::Mat::zeros(vImg.size(), CV_32F);
        Channels.push_back(ZeroChannel);
        cv::Mat MergedMat;
        cv::merge(Channels, MergedMat);

        ShowImg = MergedMat;
    }
    else
        ShowImg = vImg;

    cv::imshow(vWindowName, ShowImg);

    cv::waitKey(0);
}

void CSnowStylizer::__binarizeImg(cv::Mat& vioBinaryImg)
{
    if (!vioBinaryImg.empty())
        vioBinaryImg.release();
    if (vioBinaryImg.size() != m_OriginImage.size())
        vioBinaryImg = cv::Mat(m_OriginImage.rows, m_OriginImage.cols, CV_8UC1, cv::Scalar(0));

    cv::Mat Alpha;
    cv::extractChannel(m_OriginImage, Alpha, 3);
    cv::threshold(Alpha, vioBinaryImg, 128, 255, cv::THRESH_BINARY);
}

void CSnowStylizer::__calculatNormal(std::vector<SContourGradInfo>& vioSurfaceNormals, const cv::Mat& vBinaryImg)
{
    std::vector<std::vector<cv::Point>> Contours;
    std::vector<cv::Vec4i> Hierarchy;
    cv::findContours(vBinaryImg, Contours, Hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

    cv::Mat GradientX, GradientY;
    cv::Sobel(vBinaryImg, GradientX, CV_32F, 1, 0, 3);
    cv::Sobel(vBinaryImg, GradientY, CV_32F, 0, 1, 3);

    for (const auto& Contour : Contours)
    {
        for (const auto& Point : Contour)
        {
            int x = Point.x;
            int y = Point.y;
            float GradValueX = GradientX.at<float>(y, x);
            float GradValueY = GradientY.at<float>(y, x);
            float Magnitude = std::sqrt(GradValueX * GradValueX + GradValueY * GradValueY);
            if (GradValueY > 0)
            {
                SContourGradInfo GradInfo{x, y, GradValueY / Magnitude};
                vioSurfaceNormals.push_back(GradInfo);
            }
        }
    }
}

std::string CSnowStylizer::__formatFileName(const std::string& vFileName)
{
    std::string FileName;
    size_t LastDotPos = vFileName.find_last_of('.');
    if (LastDotPos != std::string::npos)
    {
        FileName = vFileName.substr(0, LastDotPos);
        FileName.append("_{}.png");
        return FileName;
    }
    else
        LOG_ERROR(hiveVG::TAG_KEYWORD::SNOW_STYLIZE_TAG, "File name errors.");
    return "";
}
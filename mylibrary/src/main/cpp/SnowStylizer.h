#pragma once

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/asset_manager.h>

namespace hiveVG
{
    struct SContourGradInfo
    {
        int   CoordX;
        int   CoordY;
        float GradY;
    };

    class CSnowStylizer
    {
    public:
        CSnowStylizer();
        bool loadImg(AAssetManager* vAssetManager, const std::string& vImgPath);
        bool loadImg(AAssetManager* vAssetManager, const std::string& vImgPath, const cv::Vec3b& BackgroundColor);
        bool setShapeFreq(float vFrequency);
        bool setSnowHighProportion(float vProportion);
        bool setShapeAmplitude(float vAmplitude);
        bool generateSnow(int vSnowLayerNum);
        bool setOutputPath(const std::string& vOutputPath);
        bool setOutputName(const std::string& vOutputName);

    private:
        static const float m_Pai;
        float	m_SnowHighThreshold;
        float	m_SnowMaxHighProportion;
        float	m_SnowShapeFreq;
        float	m_SnowShapeAmplitude;
        float	m_ShapeFreqMultiplier;
        float	m_ShapeAmplitudeMultiplier;
        struct
        {
            std::string _FilePath;
            std::string _FileName;
        } m_FileNameFormat;

        cv::Mat				 m_OriginImage;
        cv::Mat				 m_SnowImage;
        std::vector<cv::Mat> m_LayerSnowImgs;

        void		__saveImg();
        void		__showImg(const cv::Mat& vImg, const std::string& vWindowName = "Image browsing");
        void		__binarizeImg(cv::Mat& vioBinaryImg);
        void		__calculatNormal(std::vector<SContourGradInfo>& vioNormalList, const cv::Mat& vBinaryImg);
        void		__generateLayerSnow(int vMaxHigh, float vShapePhase, const std::vector<SContourGradInfo>& vNormalList);
        float		__getRandomFloat(float vMin, float vMax);
        int			__getRandomInt(int vSeed, int vMin, int vMax);
        void		__loadFileSavePath(const std::string& vFilePath);
        std::string __formatFileName(const std::string& vFileName);
    };
}

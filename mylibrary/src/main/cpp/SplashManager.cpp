#include <opencv2/opencv.hpp>
#include "SplashManager.h"
#include <random>
#include "TextureBlender.h"
#include "Common.h"
#include "FileUtils.h"

using namespace hiveVG;

CSplashManager::~CSplashManager()
{
    for (auto& m_SequencePlayer : m_SequencePlayers)
    {
        delete m_SequencePlayer;
    }
    if (m_pTexBlender) delete m_pTexBlender;
}

void CSplashManager::pushBack(CSequenceFramePlayer* vSequenceFramePlayer)
{
    m_SequencePlayers.push_back(vSequenceFramePlayer);
}

void CSplashManager::initSequenceState(const std::string& vImagePath, const float& vScale)
{
    __calculatePosition(vImagePath, vScale);
    m_SplashScale = vScale;
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequenceState.emplace_back(__initSequenceParams());
        m_SequencePlayers[i]->setScreenUVOffset(m_SequenceState[i]._UVOffset);
    }
}

void CSplashManager::updateFrameAndUV(double vDeltaTime)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        if (m_SequencePlayers[i]->getLoopState() || (!m_SequencePlayers[i]->getLoopState() && !m_SequencePlayers[i]->getFinishState()))
        {
            m_SequencePlayers[i]->updateFrameAndUV(vDeltaTime);
        }
    }
}

void CSplashManager::draw(CScreenQuad* vQuad)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        if (!m_SequenceState[i]._IsAlive)
            continue;
        m_SequencePlayers[i]->draw(vQuad);
    }
}

void CSplashManager::updateSequenceState(float vDeltaTime)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        auto& Player = m_SequencePlayers[i];
        auto& State = m_SequenceState[i];
        if (!State._IsAlive)
        {
            State._AlreadyDeadTime += vDeltaTime;
            if (State._AlreadyDeadTime > State._PlannedDeadTime)
            {
                State = __initSequenceParams();
                State._IsAlive = true;
                Player->setScreenUVOffset(State._UVOffset);
                Player->setScreenUVScale(glm::vec2(State._UVScale, State._UVScale));
            }
        }
        else
        {
            State._AlreadyLivingTime += vDeltaTime;
            if (State._AlreadyLivingTime > State._PlannedLivingTime)
            {
                State._IsAlive = false;
            }
        }
    }
}

void CSplashManager::setLoop(bool vLoop)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setLoopPlayback(vLoop);
    }
}

void CSplashManager::setFrameRate(int vFrameRate)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setFrameRate(vFrameRate);
    }
}

void CSplashManager::setRotationAngle(float vRotationAngle)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setRotationAngle(vRotationAngle);
    }
}

void CSplashManager::setScreenUvOffset(glm::vec2& vUVOffset)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setScreenUVOffset(vUVOffset);
    }
}

SSequenceState CSplashManager::__initSequenceParams()
{
    std::random_device Rd;
    std::mt19937 Gen(Rd());
    std::uniform_int_distribution<>  IntDistribution(0, 1);
    std::uniform_real_distribution<> FloatDistribution(0.0, 1.0);

    SSequenceState State;
    State._IsAlive = IntDistribution(Gen);

    //FloatDistribution.param(std::uniform_real_distribution<>::param_type(3.0f, 4.0f));
    //State._PlannedLivingTime = FloatDistribution(Gen);
    State._PlannedLivingTime = 1.0;

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(1.0f, 2.0f));
    State._PlannedDeadTime   = FloatDistribution(Gen);
    State._AlreadyDeadTime   = 0;
    State._AlreadyLivingTime = 0;
    State._MovingDirection   = 1;
    State._UVScale = m_SplashScale;

    std::uniform_int_distribution<size_t> Dis(0, m_SplashPositions.size() - 1);
    State._UVOffset = m_SplashPositions[Dis(Gen)];

    State._MovingSpeed = 0.0;
    return State;
}

void CSplashManager::setBlendStatus(bool vStatus)
{
    if(!m_pTexBlender)
    {
        m_IsBlend = false;
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Blending cannot be started by no blender.");
        return;
    }
    m_IsBlend = vStatus;
    if (m_IsBlend)
        LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "Turn on blending.");
    else
        LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "Turn off blending.");
}

void CSplashManager::blit(bool vIsBlitToScreen)
{
    if(m_IsBlend)
    {
        m_pTexBlender->blit(vIsBlitToScreen);
    }
}

bool CSplashManager::initBlender(int vWidth, int vHeight)
{
    m_pTexBlender = new CTextureBlender();
    return m_pTexBlender->init(vWidth, vHeight);
}

void CSplashManager::__calculatePosition(const std::string& vImagePath, const float& vScale)
{
    auto pAsset = CFileUtils::openFile(vImagePath.c_str());
    if (!pAsset) return;
    size_t FileSize = CFileUtils::getFileBytes(pAsset);
    std::vector<uchar> Buffer(FileSize);
    int Flag = CFileUtils::readFile<uchar>(pAsset, Buffer.data(), FileSize);
    if (Flag < 0) return;
    CFileUtils::closeFile(pAsset);

    cv::Mat OriginImage = cv::imdecode(Buffer, cv::IMREAD_UNCHANGED);
    cv::Mat MarkedImg = OriginImage.clone();
    if (OriginImage.empty())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SPLASH_MANAGER_TAG, "读取图像失败！");
    }

    cv::Mat Alpha, BinaryImg;
    cv::extractChannel(OriginImage, Alpha, 3);
    cv::threshold(Alpha, BinaryImg, 128, 255, cv::THRESH_BINARY);

    cv::Mat BinaryCopy = BinaryImg.clone();
    std::vector<std::vector<cv::Point>> Contours;
    std::vector<cv::Vec4i> Hierarchy;
    cv::findContours(BinaryImg, Contours, Hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    std::vector<cv::Point> HorizontalPoints;
    const int SegmentSize = 20; // 每段线包含的点数
    const float AngleThreshold = 1.0f; // 角度容差

    for (const auto& Contour : Contours)
    {
        int N = Contour.size();
        if (N < SegmentSize)
            continue;

        for (int i = 0; i <= N - SegmentSize; i += SegmentSize)
        {
            std::vector<cv::Point> Segment(Contour.begin() + i, Contour.begin() + i + SegmentSize);

            // 拟合这段线
            cv::Vec4f Line;
            cv::fitLine(Segment, Line, 2, 0, 0.01, 0.01);

            float Dx = Line[0];
            float Dy = Line[1];
            float Angle = std::atan2(Dy, Dx) * 180.0f / CV_PI;
            if (std::abs(Angle) < AngleThreshold || std::abs(Angle - 180.0f) < AngleThreshold || std::abs(Angle + 180.0f) < AngleThreshold)
            {
                HorizontalPoints.insert(HorizontalPoints.end(), Segment.begin(), Segment.end());
            }
        }
    }

    cv::Mat GradX, GradY;
    cv::Sobel(BinaryCopy, GradX, CV_32F, 1, 0, 3);
    cv::Sobel(BinaryCopy, GradY, CV_32F, 0, 1, 3);

    std::vector<cv::Point> UpwardNormals;
    for (const auto& Point : HorizontalPoints)
    {
        int X = Point.x;
        int Y = Point.y;

        float Dx = GradX.at<float>(Y, X);
        float Dy = GradY.at<float>(Y, X);
        float Mag = std::sqrt(Dx * Dx + Dy * Dy);
        if (Mag < 1e-3) continue; // 避免除以0

        // 单位法线
        float Ny = Dy / Mag;
        if (Ny == 1)
        {
            UpwardNormals.push_back(Point);
        }
    }

    std::vector<cv::Point> SelectedPoints;
    int MaxPoints = 50;
    if (UpwardNormals.size() > MaxPoints)
    {
        int Step = UpwardNormals.size() / MaxPoints;

        for (int i = 0; i < MaxPoints; ++i)
        {
            SelectedPoints.push_back(UpwardNormals[i * Step]);
        }
    }
    else
    {
        SelectedPoints = UpwardNormals;
    }

    m_SplashPositions.reserve(SelectedPoints.size());
    auto Width  = static_cast<float>(OriginImage.cols);
    auto Height = static_cast<float>(OriginImage.rows);
    auto Delta = vScale * Height * 0.5f;
    for (const auto& Point : SelectedPoints)
    {
        float X = Point.x / Width * 2.0f - 1.0f;
        float Y = 1.0f - ((Point.y - Delta) / Height) * 2.0f;
        m_SplashPositions.emplace_back(glm::vec2(X, Y));
    }

    LOG_INFO(hiveVG::TAG_KEYWORD::SPLASH_MANAGER_TAG, "总轮廓点数: %d", SelectedPoints.size());
    for (const auto& Point : SelectedPoints)
    {
        cv::circle(MarkedImg, Point, 3, cv::Scalar(0, 0, 255, 255), -1);
        LOG_INFO(hiveVG::TAG_KEYWORD::SPLASH_MANAGER_TAG, "(%d, %d)", Point.x, Point.y);
    }

    std::string OutputPath = "/sdcard/Download/contour_marked.png";
    bool Success = cv::imwrite(OutputPath, MarkedImg);
    if (Success)
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SPLASH_MANAGER_TAG, "轮廓可视化图已保存到: %s", OutputPath.c_str());
    }
    else
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SPLASH_MANAGER_TAG, "保存图像失败！");
    }
}
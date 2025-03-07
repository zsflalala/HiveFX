#include "BlendManager.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "TextureBlender.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"
#include "SlideWindow.h"

using namespace hiveVG;

CBlendManager::CBlendManager(AAssetManager *vAssetManager) : m_pAssetManager(vAssetManager)
{
    CTextureBlender::setAssetManager( vAssetManager );
}

CBlendManager::~CBlendManager()
{
    if (m_pScreenQuad) delete m_pScreenQuad;
    if (m_pTexBlender) delete m_pTexBlender;

    for (auto& pPlayer : m_PlayerList) {
        std::visit([&pPlayer](auto p) {
            using T = decltype(p);
            if constexpr (
                    std::is_same_v<T, CSingleTexturePlayer*> ||
                    std::is_same_v<T, CSequenceFramePlayer*> ||
                    std::is_same_v<T, CBillBoardManager*> ||
                    std::is_same_v<T, CSlideWindow*>
                    ) {
                delete p;
                pPlayer = nullptr; // 设为 nullptr_t
            }
        }, pPlayer);
    }
    m_PlayerList.clear();
}

bool CBlendManager::init(const std::string& vFilePath, int vWidth, int vHeight)
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_Width = vWidth;
    m_Height = vHeight;
    return __initBlender() && __initPlayer(vFilePath);
}

void CBlendManager::render()
{
    if(m_PlayerList.size() == 0 || !m_pTexBlender->isInit())
    {
        LOG_ERROR(TAG_KEYWORD::BLENDER_MANAGER_TAG, "Error occurred in rendering");
        return;
    }

    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    std::function<void()> DrawCallFunc;

    for(int i = m_PlayerList.size() - 1; i >= 0; i--)
    {
        if(!m_RenderStatus[i])
            continue;
        EBlendingMode::EBlendingMode BlendMode = m_BlendModeList[i];
        if (CSingleTexturePlayer** p = std::get_if<CSingleTexturePlayer*>(&m_PlayerList[i]))
        {
            DrawCallFunc = std::bind(&CBlendManager::__SingleTexDrawCallFunc, this, *p);
            __draw(DrawCallFunc, BlendMode);
        }
        if (CSequenceFramePlayer** p = std::get_if<CSequenceFramePlayer*>(&m_PlayerList[i]))
        {
            DrawCallFunc = std::bind(&CBlendManager::__SequenceFrameDrawCallFunc, this, *p, DeltaTime);
            __draw(DrawCallFunc, BlendMode);
        }
        if (CBillBoardManager** p = std::get_if<CBillBoardManager*>(&m_PlayerList[i]))
        {
            DrawCallFunc = std::bind(&CBlendManager::__BillBoardDrawCallFunc, this, *p, DeltaTime, BlendMode);
            __draw(DrawCallFunc, BlendMode);
        }
        if (CSlideWindow** p = std::get_if<CSlideWindow*>(&m_PlayerList[i]))
        {
            DrawCallFunc = std::bind(&CBlendManager::__SlideWindowDrawCallFunc, this, *p, DeltaTime);
            __draw(DrawCallFunc, BlendMode);
        }
    }
    if(m_IsBlendValid)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_pTexBlender->blit();
    }
}

bool CBlendManager::__initPlayer(const std::string &vFilePath)
{
    CJsonReader JsonReader = CJsonReader(m_pAssetManager, vFilePath);
    int Layer = 1;
    while(true)
    {
        Json::Value LayerConfig;
        try
        {
            LayerConfig = JsonReader.getObject(std::to_string(Layer));
        }
        catch (std::runtime_error& e)
        {
            break;
        }
        // TODO: 标签校验
        std::string LayerTypeStr = LayerConfig["layer_type"].asString();
        std::string BlendingModeStr = LayerConfig["blend_mode"].asString();
        Json::Value Config = LayerConfig["config"];

        ELayerType::ELayerType LayerType = ELayerType::FromString(LayerTypeStr);
        EBlendingMode::EBlendingMode BlendingMode = EBlendingMode::FromString(BlendingModeStr);

        switch(LayerType)
        {
            case ELayerType::SEQUENCE_FRAME:
                __createLayerPlayer(std::bind(&CBlendManager::__createSequenceFramePlayer,this, Config), BlendingMode);
                break;
            case ELayerType::BILLBOARD:
                __createLayerPlayer(std::bind(&CBlendManager::__createBillBoardManager,this, Config), BlendingMode);
                break;
            case ELayerType::SINGLE_PICTURE:
                __createLayerPlayer(std::bind(&CBlendManager::__createSingleTexPlayer,this, Config), BlendingMode);
                break;
            case ELayerType::SLIDE_WINDOW:
                __createLayerPlayer(std::bind(&CBlendManager::__createSlideWindow, this, Config), BlendingMode);
        }
        Layer++;
    }
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    return true;
}

bool CBlendManager::__initBlender()
{
    m_pTexBlender = new CTextureBlender();
    return m_pTexBlender->init(m_Width, m_Height);
}

void CBlendManager::__createLayerPlayer(std::function<LayerPlayer()> vFunc, EBlendingMode::EBlendingMode vBlendMode)
{
    LayerPlayer pPlayer = vFunc();
    if (!std::holds_alternative<std::nullptr_t>(pPlayer))
    {
        m_PlayerList.push_back(pPlayer);
        m_BlendModeList.push_back(vBlendMode);
        m_RenderStatus.push_back(true);
    }
}

void CBlendManager::__draw(const std::function<void()>& vDrawCall, EBlendingMode::EBlendingMode vBlendMode)
{
    if(vBlendMode == EBlendingMode::NONE)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        vDrawCall();
    }
    else
    {
        m_pTexBlender->setBlendingMode(vBlendMode);
        m_pTexBlender->drawAndBlend(vDrawCall);
    }
}

LayerPlayer CBlendManager::__createSingleTexPlayer(const Json::Value &vConfig)
{
    std::string FramePath = vConfig["frames_path"].asString();
    CSingleTexturePlayer* pPlayer = new CSingleTexturePlayer(FramePath);
    pPlayer->initTextureAndShaderProgram(m_pAssetManager);
    return pPlayer;
}

LayerPlayer CBlendManager::__createSequenceFramePlayer(const Json::Value &vConfig)
{
    std::string FramePath    = vConfig["frames_path"].asString();
    std::string FrameType    = vConfig["frames_type"].asString();
    int         FrameCount   = vConfig["frames_count"].asInt();
    int         SequenceRows = vConfig["ranks"]["rows"].asInt();
    int         SequenceCols = vConfig["ranks"]["cols"].asInt();
    std::string PlayMode     = vConfig["play_mode"].asString();

    int         PlayFPS      = vConfig["fps"].asInt();
    bool        IsLoop       = vConfig["loop"].asBool();

    EPictureType::EPictureType PictureType = EPictureType::FromString(FrameType);
    EPlayType::EPlayType PlayType = EPlayType::FromString(PlayMode);

    CSequenceFramePlayer* pPlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, PictureType);
    if(!pPlayer->initTextureAndShaderProgram(m_pAssetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return nullptr;
    }
    pPlayer->setFrameRate(PlayFPS);
    pPlayer->setLoopPlayback(IsLoop);
    if (PlayType == EPlayType::PARTIAL)
    {
        glm::vec2   UVOffset     = glm::vec2(vConfig["position"]["x"].asFloat(),
                                             vConfig["position"]["y"].asFloat());
        float       Scale        = vConfig["scale"].asFloat();
        glm::vec2   MoveSpeed    = glm::vec2(vConfig["moving_speed"][0].asFloat(),
                                             vConfig["moving_speed"][1].asFloat());

        pPlayer->setIsMoving(true);
        pPlayer->setScreenUVOffset(UVOffset);
        pPlayer->setScreenUVScale(glm::vec2(Scale, Scale));
        pPlayer->setScreenUVMovingSpeed(MoveSpeed);
    }

    return pPlayer;
}

LayerPlayer CBlendManager::__createBillBoardManager(const Json::Value &vConfig)
{
    CBillBoardManager* pManager = new CBillBoardManager();

    CJsonReader JsonReader = CJsonReader(vConfig);
    int Layer = 1;
    while(true)
    {
        Json::Value LayerConfig;
        try
        {
            LayerConfig = JsonReader.getObject(std::to_string(Layer));
        }
        catch (std::runtime_error& e)
        {
            break;
        }
        // TODO: 标签校验
        std::string LayerTypeStr = LayerConfig["layer_type"].asString();
        std::string BlendingModeStr = LayerConfig["blend_mode"].asString();
        Json::Value Config = LayerConfig["config"];

        ELayerType::ELayerType LayerType = ELayerType::FromString(LayerTypeStr);
        EBlendingMode::EBlendingMode BlendingMode = EBlendingMode::FromString(BlendingModeStr);
        // TODO: billboardManager 内添加每个序列帧的 blending mode

        if(LayerType != ELayerType::SEQUENCE_FRAME)
        {
            LOG_ERROR(TAG_KEYWORD::BLENDER_MANAGER_TAG, "Error layer type in billboard.");
        }
        else
        {
            auto VariantPlayer = __createSequenceFramePlayer(Config);
            if (CSequenceFramePlayer** p = std::get_if<CSequenceFramePlayer*>(&VariantPlayer))
                pManager->pushBack(*p);
        }
        Layer++;
    }

    pManager->initSequenceState();
    pManager->initBlender(m_Width, m_Height);
    pManager->setBlendStatus(true);

    return pManager;
}

LayerPlayer CBlendManager::__createSlideWindow(const Json::Value &vConfig)
{
    std::string PicturePath    = vConfig["picture_path"].asString();
    std::string FrameType      = vConfig["frames_type"].asString();
    std::string SlideDirection = vConfig["slide_direction"].asString();
    int  SlideSpeed = vConfig["slide_speed"].asInt();
    bool IsLoop     = vConfig["loop"].asBool();

    CSlideWindow* pSlideWindowPlayer = new CSlideWindow(PicturePath, SlideSpeed, SlideDirection);
    pSlideWindowPlayer->createProgram(m_pAssetManager);
    pSlideWindowPlayer->loadTextures(m_pAssetManager);
    return pSlideWindowPlayer;
}

void CBlendManager::__SingleTexDrawCallFunc(CSingleTexturePlayer *vSingleTexturePlayer)
{
    vSingleTexturePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
}

void CBlendManager::__SequenceFrameDrawCallFunc(CSequenceFramePlayer *vSequFraPlayer, double vDeltaTime)
{
    vSequFraPlayer->updateFrameAndUV(m_Width, m_Height, vDeltaTime);
    vSequFraPlayer->draw(m_pScreenQuad);
}

void CBlendManager::__BillBoardDrawCallFunc(CBillBoardManager *vBillBoardManager, double vDeltaTime, EBlendingMode::EBlendingMode vMode)
{
    vBillBoardManager->updateFrameAndUV(m_Width, m_Height, vDeltaTime);
    vBillBoardManager->updateSequenceState(vDeltaTime);
    vBillBoardManager->setFrameRate(8.0f);
    static int PlayersNum = vBillBoardManager->getSequencePlayerLength();
    static std::vector<glm::vec2> ScreenUVScale(PlayersNum, glm::vec2(1.0f, 1.0f));
    for (int i = 0; i < PlayersNum; i++)
    {
        ScreenUVScale[i].y = ScreenUVScale[i].x / vBillBoardManager->getImageAspectRatioAt(i);
        vBillBoardManager->setImageAspectRatioAt(i, ScreenUVScale[i]);
    }
    vBillBoardManager->draw(m_pScreenQuad);
    if(vMode == EBlendingMode::NONE)
        vBillBoardManager->blit(true);
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pTexBlender->getSrcFBO());
        vBillBoardManager->blit(false);
    }
}

void CBlendManager::__SlideWindowDrawCallFunc(CSlideWindow *vSlideWindowPlayer, double vDeltaTime)
{
    vSlideWindowPlayer->updateFrameAndDraw(m_Width, m_Height, vDeltaTime * 100.0f,m_pScreenQuad);
}

void CBlendManager::setBlendModeByIndex(EBlendingMode::EBlendingMode vMode, int vIndex)
{
    assert(vIndex < m_BlendModeList.size());
    if(vIndex >= m_BlendModeList.size())
    {
        LOG_ERROR(TAG_KEYWORD::BLENDER_MANAGER_TAG, "Failed to set blend mode with out-of-range index.");
        return;
    }
    m_BlendModeList[vIndex] = vMode;
    if (CBillBoardManager** p = std::get_if<CBillBoardManager*>(&m_PlayerList[vIndex]))
    {
        if(vMode == EBlendingMode::NONE)
            (*p)->setBlendStatus(false);
        else
            (*p)->setBlendStatus(true);
    }
}

void CBlendManager::setBlendModeForAllLayer(EBlendingMode::EBlendingMode vMode)
{
    for(int i = 0; i< m_BlendModeList.size(); i++)
    {
        m_BlendModeList[i] = vMode;
        if (CBillBoardManager** p = std::get_if<CBillBoardManager*>(&m_PlayerList[i]))
        {
            if(vMode == EBlendingMode::NONE)
                (*p)->setBlendStatus(false);
            else
                (*p)->setBlendStatus(true);
        }
    }
    if(vMode == EBlendingMode::NONE)
        m_IsBlendValid = false;
    else
        m_IsBlendValid = true;
}

void CBlendManager::switchRenderStatus(int vIndex)
{
    assert(vIndex < m_RenderStatus.size());
    if(vIndex >= m_BlendModeList.size())
    {
        LOG_ERROR(TAG_KEYWORD::BLENDER_MANAGER_TAG, "Index out of range.");
        return;
    }
    m_RenderStatus[vIndex] = !m_RenderStatus[vIndex];
}
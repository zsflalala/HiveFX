#include "Common.h"
#include <android/asset_manager.h>
#include <json/json.h>
#include <functional>
#include <vector>

namespace hiveVG
{

    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;
    class CBillBoardManager;
    class CSlideWindow;
    class CTextureBlender;

    using LayerPlayer = std::variant<CSingleTexturePlayer*,
                                     CSequenceFramePlayer*,
                                     CBillBoardManager*,
                                     CSlideWindow*,
                                     std::nullptr_t>;
    class CBlendManager
    {
    public:
        CBlendManager(AAssetManager *vAssetManager);
        ~CBlendManager();

        bool init(const std::string& vFilePath, int vWidth, int vHeight);
        void render();
        void setBlendModeByIndex(EBlendingMode::EBlendingMode vMode, int vIndex);
        void switchRenderStatus(int vIndex);

    private:
        bool __initPlayer(const std::string &vFilePath);
        bool __initBlender();
        void __createLayerPlayer(std::function<LayerPlayer()> vFunc, EBlendingMode::EBlendingMode vBlendMode);

        LayerPlayer __createSingleTexPlayer(const Json::Value& vConfig);
        LayerPlayer __createSequenceFramePlayer(const Json::Value& vConfig);
        LayerPlayer __createBillBoardManager(const Json::Value& vConfig);
        LayerPlayer __createSlideWindow(const Json::Value& vConfig);

        void __SingleTexDrawCallFunc(CSingleTexturePlayer* vSingleTexturePlayer);
        void __SequenceFrameDrawCallFunc(CSequenceFramePlayer* vSequFraPlayer, double vDeltaTime);
        void __BillBoardDrawCallFunc(CBillBoardManager* vBillBoardManager, double vDeltaTime);
        void __SlideWindowDrawCallFunc(CSlideWindow* vSlideWindowPlayer, double vDeltaTime);

        double m_LastFrameTime = 0.0f;
        double m_CurrentTime   = 0.0f;
        // TODO: 全局管理一份屏幕长宽
        int    m_Width;
        int    m_Height;

        AAssetManager*   m_pAssetManager = nullptr;
        CScreenQuad*     m_pScreenQuad   = nullptr;
        CTextureBlender* m_pTexBlender   = nullptr;

        std::vector<bool>        m_RenderStatus;
        std::vector<LayerPlayer> m_PlayerList;
        std::vector<EBlendingMode::EBlendingMode> m_BlendModeList;
    };
}

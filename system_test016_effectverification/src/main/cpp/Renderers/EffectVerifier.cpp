#include "EffectVerifier.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <cassert>
#include "Common.h"
#include "AppContext.h"
#include "EffectTester.h"

using namespace hiveVG;

CEffectVerifier::CEffectVerifier(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CEffectVerifier::~CEffectVerifier()
{
    if(m_pTester)
    {
        delete m_pTester;
        m_pTester = nullptr;
    }
}

void CEffectVerifier::saveTexture(const std::string& vFileName)
{
    m_pTester->saveScreenBuffer(vFileName);
}

void CEffectVerifier::__initAlgorithm()
{
    m_pTester = new CEffectTester();
    m_pTester->setFilePath(m_pApp->activity->externalDataPath);

    int Width = 0, Height = 0;
    assert(m_pApp->window != nullptr);
    if (m_pApp->window)
    {
        Width = ANativeWindow_getWidth(m_pApp->window);
        Height = ANativeWindow_getHeight(m_pApp->window);
    }
    m_pTester->init(Width, Height);
}

void CEffectVerifier::verify()
{
    double PSNR, SSIM;
    m_pTester->evaluateImagesQuality("RainStormFore_NB_2582x1220.png", "RainStormFore_NB_645x305.png",PSNR,SSIM);
    LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "PSNR: %lf, SSIM: %lf", PSNR, SSIM);
}

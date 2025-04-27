#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CEffectTester;
    class CEffectVerifier
    {
    public:
        CEffectVerifier(android_app *vApp);
        ~CEffectVerifier();

        void saveTexture(const std::string& vFileName);
        void verify();

    private:
        void __initAlgorithm();

        android_app*   m_pApp    = nullptr;
        CEffectTester* m_pTester = nullptr;
    };
}
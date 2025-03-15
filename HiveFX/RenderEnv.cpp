#include "RenderEnv.h"
#include "Common.h"
#include "logging.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

using namespace hiveTest;

hiveTest::CRenderEnv::CRenderEnv()
{
}

hiveTest::CRenderEnv::~CRenderEnv()
{
    glfwTerminate();
}

bool hiveTest::CRenderEnv::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_pWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "UnitTest", NULL, NULL);
    if (!m_pWindow)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::UNIT_TEST_TAG, "Failed to create GLFW window.");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_pWindow);
    glfwSetFramebufferSizeCallback(m_pWindow, &CRenderEnv::__windowSizeCallBack);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::UNIT_TEST_TAG, "Failed to initialize GLAD.");
        return false;
    }
	return false;
}

void hiveTest::CRenderEnv::render(const std::function<void()>& vDrawCall)
{
    while (!glfwWindowShouldClose(m_pWindow))
    {
        __processInput();

        vDrawCall();

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }
}

void hiveTest::CRenderEnv::__processInput()
{
    if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_pWindow, true);
}

void hiveTest::CRenderEnv::__windowSizeCallBack(GLFWwindow* vWindow, int vWidth, int vHeight)
{
    glViewport(0, 0, vWidth, vHeight);
}
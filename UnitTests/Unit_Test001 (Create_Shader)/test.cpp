#include "pch.h"
#include <fmt/core.h>

std::string AssetsPath = "..\\..\\UnitTests\\asserts\\shaders\\";

using namespace hiveVG;
using namespace hiveTest;

struct STestEnv
{
	std::string _VertFilePath;
	std::string _FragFilePath;
	std::function<void(const std::string&, const std::string&)> _ValidationFunc;
};

class CCreateShaderTestFixture : public testing::TestWithParam<struct STestEnv>
{
public:
	static CRenderEnv* m_pRenderEnv;

	static void SetUpTestCase()
	{
		char buffer[MAX_PATH];
		DWORD result = GetCurrentDirectoryA(MAX_PATH, buffer);
		assert(!printf(buffer));
		m_pRenderEnv = new CRenderEnv();
		m_pRenderEnv->init();
		std::cout << fmt::format("{}", 2) << std::endl;
	}

	static void TearDownTestCase()
	{
		delete m_pRenderEnv;
	}

protected:
	STestEnv m_TestEnv;

	void SetUp() override
	{
		m_TestEnv = GetParam();
	}
};

CRenderEnv* CCreateShaderTestFixture::m_pRenderEnv = nullptr;

void NT_CompileShaderCorrectly(const std::string& vVert, const std::string& vFrag)
{
	CShaderProgram* pCorrectShader = CShaderProgram::createProgram(vVert, vFrag);
	EXPECT_TRUE(pCorrectShader);
	EXPECT_TRUE(pCorrectShader->m_ProgramHandle > 0);
	EXPECT_NE(pCorrectShader->__getOrCreateUniformId("screenUVScale"),-1);
}

void DT_InvalidShader(const std::string& vVert, const std::string& vFrag)
{
	CShaderProgram* pCorrectShader = CShaderProgram::createProgram(vVert, vFrag);
	EXPECT_FALSE(pCorrectShader);
}

STestEnv g_CorrectShader = STestEnv{ ._VertFilePath{AssetsPath + "sequenceTexturePlayer.vert"},
									 ._FragFilePath{AssetsPath + "sequenceTexturePlayer.frag"},
									 ._ValidationFunc{NT_CompileShaderCorrectly} };

STestEnv g_InCorrectShader = STestEnv{ ._VertFilePath{AssetsPath + "errorShader.vert"},
									   ._FragFilePath{AssetsPath + "errorShader.frag"},
									   ._ValidationFunc{DT_InvalidShader} };

STestEnv g_InvalidFilePath = STestEnv{ ._VertFilePath{AssetsPath + "notExistShader.vert"},
									   ._FragFilePath{AssetsPath + "sequenceTexturePlayer.frag"},
									   ._ValidationFunc{DT_InvalidShader} };

TEST_P(CCreateShaderTestFixture, Test_CreateShader)
{ 
	m_TestEnv._ValidationFunc(m_TestEnv._VertFilePath, m_TestEnv._FragFilePath);
}

INSTANTIATE_TEST_CASE_P(CreateShader, CCreateShaderTestFixture, testing::Values(
	g_CorrectShader,
	g_InCorrectShader,
	g_InvalidFilePath
));

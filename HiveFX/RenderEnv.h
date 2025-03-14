#pragma once

class GLFWwindow;

namespace hiveTest
{
	class CRenderEnv
	{
	public:
		CRenderEnv();
		~CRenderEnv();

		bool init();
		void render();

	private:
		void __processInput();
		static void __windowSizeCallBack(GLFWwindow* vWindow, int vWidth, int vHeight);

		GLFWwindow* m_pWindow = nullptr;
	};
}
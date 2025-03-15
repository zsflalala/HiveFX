#pragma once
#include <functional>

struct GLFWwindow;

namespace hiveTest
{
	class CRenderEnv
	{
	public:
		CRenderEnv();
		~CRenderEnv();

		bool init();
		void render(const std::function<void()>& vDrawCall);

		GLFWwindow* fetchWindow() { return m_pWindow; }

	private:
		void __processInput();
		static void __windowSizeCallBack(GLFWwindow* vWindow, int vWidth, int vHeight);

		GLFWwindow* m_pWindow = nullptr;
	};
}
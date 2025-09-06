#pragma once
#include "../Window/Window.h"

#include <string>
#include <chrono>

namespace RTL {

	class Application {
	public:
        Application(const std::string name, const int width, const int height);
		~Application();

		void Run();

	private:
		void Init();
		void Terminate();

		void OnUpdate();

	private:
		std::string m_Name;
		int m_Width, m_Height;

		Framebuffer m_Framebuffer;
		Window* m_Window;
		std::chrono::steady_clock::time_point m_LastFrameTime;
	};

}

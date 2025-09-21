#pragma once

#include "RTL/Window/Window.h"
#include "RTL/Renderer/Renderer.h"
#include "RTL/Shader/BlinnShader.h"

#include <chrono>
#include <string>

namespace RTL {

	struct Camera {
		Vec4 Pos = Vec4(0.0f, 0.0f, 0.01f, 1.0f);
		Vec4 Right = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
		Vec4 Up = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
		Vec4 Dir = Vec4(0.0f, 0.0f, -1.0f, 0.0f);
		float Aspect = 4.0f / 4.0f;
	};

	class Application {
	public:
		Application(
			int argc, char* argv[],
			const std::string& name,
			const int width, const int height);

		~Application();

		void Run();

	private:
		void Init();
		void Terminate();

		void OnCameraUpdate(float time);
		void OnUpdate(float time);

	private:
		std::string m_Name;
		int m_Width, m_Height;
		std::chrono::steady_clock::time_point m_LastFrameTime;

		Window* m_Window;
		Framebuffer* m_Framebuffer;

		Camera m_Camera;
	};

}

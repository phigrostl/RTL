#pragma once
#include "Window/Window.h"
#include "Renderer/Renderer.h"
#include "Shaders/BlinnShader.h"

#include <string>
#include <chrono>

namespace RTL {

	struct Camera {
		Vec4 Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
		Vec4 Right = { 1.0f, 0.0f, 0.0f, 0.0f };
		Vec4 Up = { 0.0f, 1.0f, 0.0f, 0.0f };
		Vec4 Dir = { 0.0f, 0.0f, -1.0f, 0.0f };
		float Aspect = 1.0f;
	};

	class Application {
	public:
        Application(const std::string name, const int width, const int height);
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

		Window* m_Window;
		Framebuffer m_Framebuffer{ m_Width, m_Height };

		std::chrono::steady_clock::time_point m_LastFrameTime;

		Program<BlinnVertex, BlinnUniforms, BlinnVaryings> m_Program{ BlinnVertexShader, BlinnFragmentShader };
		BlinnUniforms m_Uniforms;

		Camera m_Camera;
	};

}

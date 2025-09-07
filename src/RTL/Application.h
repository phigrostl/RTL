#pragma once
#include "Window/Window.h"
#include "Renderer/Renderer.h"
#include "Shaders/BlinnShader.h"

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

		void OnUpdate(float time);

	private:
		std::string m_Name;
		int m_Width, m_Height;

		Window* m_Window;
		Framebuffer m_Framebuffer{ m_Width, m_Height };

		std::chrono::steady_clock::time_point m_LastFrameTime;

		Program<BlinnVertex, BlinnUniforms, BlinnVaryings> m_Program{ BlinnVertexShader };
		BlinnUniforms m_Uniforms;

	};

}

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "RTL/Window/Window.h"
#include "RTL/Renderer/Renderer.h"
#include "RTL/Shader/BlinnShader.h"

#include <chrono>
#include <string>
#include <fstream>
#include <thread>

namespace RTL {

	struct Camera {
		Vec4 Pos = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 Right = Vec4(-1.0f, 0.0f, 0.0f, 0.0f);
		Vec4 Up = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
		Vec4 Dir = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
		Vec3 Rot = Vec3(0, 0, 0);
		float Aspect = 4.0f / 4.0f;
		float Fov = PI / 2.0f;
		float Near = 0.1f;
		float Far = 100.0f;
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

		void LoadMesh(const char* fileName);

	private:
		std::string m_Name;
		int m_Width, m_Height;
		std::chrono::steady_clock::time_point m_LastFrameTime;

		Window* m_Window;
		Framebuffer* m_Framebuffer;

		Camera m_Camera;
		std::vector<Triangle<BlinnVertex>> m_Mesh;

		BlinnUniforms m_Uniforms;
		Program<BlinnVertex, BlinnUniforms, BlinnVaryings> m_Program{ BlinnVertexShader, BlinnFragmentShader };
	};

}

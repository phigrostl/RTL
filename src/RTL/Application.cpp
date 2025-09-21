#include "Application.h"

namespace RTL {

	Application::Application(
		int argc, char* argv[],
		const std::string& name,
		const int width, const int height)
		: m_Name(name), m_Width(width), m_Height(height) {

		Init();
	}

	Application::~Application() {
		Terminate();
	}

	void Application::Init() {
		Window::Init();
		m_Window = Window::Create(m_Name, m_Width, m_Height);
		m_Framebuffer = Framebuffer::Create(m_Width, m_Height);
	}

	void Application::Terminate() {
		delete m_Window;
		Window::Terminate();
	}

	void Application::Run() {
		while (!m_Window->Closed()) {
			m_Framebuffer->Clear();
			m_Framebuffer->ClearDepth();
			m_Window->PollInputEvents();
			OnUpdate();
			m_Window->DrawFramebuffer(m_Framebuffer);
		}
	}

	void Application::OnUpdate() {
		Program<BlinnVertex, BlinnUniforms, BlinnVaryings> program(BlinnVertexShader);
		Triangle<BlinnVertex> tri;
		tri.Vertex[0].ModelPos = { -10.0f, 10.0f, -10.0f, 1.0f };
		tri.Vertex[1].ModelPos = { -10.0f, -10.0f, -10.0f, 1.0f };
		tri.Vertex[2].ModelPos = { 30.0f, -10.0f, -10.0f, 1.0f };
		BlinnUniforms uniforms;
		uniforms.MVP = Mat4Perspective(PI / 2.0f, 1.0f, 1.0f, 10.0f);

		Renderer::Draw(m_Framebuffer, program, tri, uniforms);
	}

}

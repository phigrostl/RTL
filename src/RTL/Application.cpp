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

			float deltaTime = (std::chrono::steady_clock::now() - m_LastFrameTime).count() * 0.001f * 0.001f;

			m_LastFrameTime = std::chrono::steady_clock::now();

			OnUpdate(deltaTime);
			m_Window->DrawFramebuffer(m_Framebuffer);
		}
	}

	void Application::OnCameraUpdate(float time) {
		constexpr float speed = 0.001f;
		if (m_Window->GetKey(RTL_KEY_W))
			m_Camera.Pos = m_Camera.Pos + m_Camera.Dir * speed * time;
		if (m_Window->GetKey(RTL_KEY_S))
			m_Camera.Pos = m_Camera.Pos - m_Camera.Dir * speed * time;
		if (m_Window->GetKey(RTL_KEY_A))
			m_Camera.Pos = m_Camera.Pos - m_Camera.Right * speed * time;
		if (m_Window->GetKey(RTL_KEY_D))
			m_Camera.Pos = m_Camera.Pos + m_Camera.Right * speed * time;

		constexpr float rotateSpeed = 0.001f;
		Mat4 rotation = Mat4Identity();
		if (m_Window->GetKey(RTL_KEY_Q))
			rotation = Mat4RotateY(rotateSpeed * time);
		if (m_Window->GetKey(RTL_KEY_E))
			rotation = Mat4RotateY(-rotateSpeed * time);
		m_Camera.Dir = rotation * m_Camera.Dir;
		m_Camera.Right = rotation * m_Camera.Right;
	}

	void Application::OnUpdate(float time) {
		OnCameraUpdate(time);

		Program<BlinnVertex, BlinnUniforms, BlinnVaryings> program(BlinnVertexShader, BlinnFragmentShader);
		Triangle<BlinnVertex> tri;

		Mat4 view = Mat4LookAt(m_Camera.Pos, m_Camera.Pos + m_Camera.Dir, m_Camera.Up);
		Mat4 proj = Mat4Perspective(PI / 2.0f, m_Camera.Aspect, 0.1f, 10.0f);

		BlinnUniforms uniforms;
		uniforms.MVP = proj * view;
		

		tri[0].ModelPos = { -10.0f, 10.0f, -10.0f, 1.0f };
		tri[1].ModelPos = { -10.0f, -10.0f, -10.0f, 1.0f };
		tri[2].ModelPos = { 1.0f, -1.0f, -1.0f, 1.0f };

		Renderer::Draw(m_Framebuffer, program, tri, uniforms);
	}

}

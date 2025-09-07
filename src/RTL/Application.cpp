#include "Application.h"

#include "Window/Window.h"

#include <string>
#include <memory>

namespace RTL {

	Application::Application(const std::string name, const int width, const int height)
		: m_Name(name), m_Width(width), m_Height(height) {

		Init();
	}

	Application::~Application() {
		Terminate();
	}

	void Application::Init() {
		Window::Init();
		m_Window = Window::Create(m_Name, m_Width, m_Height);
		m_Framebuffer.Clear();
		m_Window->DrawFramebuffer(m_Framebuffer);
	}

	void Application::Terminate() {
		delete m_Window;
		Window::Terminate();
	}

	void Application::Run() {
		while (!m_Window->Closed()) {
			auto nowFrameTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowFrameTime - m_LastFrameTime);
			float time = duration.count() * 0.001f * 0.001f;
			Window::PollInputEvents();
			OnUpdate(time);
		}
	}

	void Application::OnCameraUpdate(float time) {
		constexpr float speed = 1.0f;
		if (m_Window->GetKey(RTL_KEY_W))
			m_Camera.Pos = m_Camera.Pos + m_Camera.Dir * speed * time;
		if (m_Window->GetKey(RTL_KEY_S))
			m_Camera.Pos = m_Camera.Pos - m_Camera.Dir * speed * time;
		if (m_Window->GetKey(RTL_KEY_A))
			m_Camera.Pos = m_Camera.Pos - m_Camera.Right * speed * time;
		if (m_Window->GetKey(RTL_KEY_D))
			m_Camera.Pos = m_Camera.Pos + m_Camera.Right * speed * time;
		if (m_Window->GetKey(RTL_KEY_SPACE))
			m_Camera.Pos = m_Camera.Pos + m_Camera.Up * speed * time;
		if (m_Window->GetKey(RTL_KEY_LEFT_SHIFT))
			m_Camera.Pos = m_Camera.Pos - m_Camera.Up * speed * time;

		constexpr float rotateSpeed = 0.01f;
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

		m_Framebuffer.Clear();
		m_Framebuffer.ClearDepth(100.0f);

		if (m_Window->GetKey(RTL_KEY_0))
			printf("0\n");

		Triangle<BlinnVertex> tri;

		Mat4 view = Mat4LookAt(m_Camera.Pos, m_Camera.Pos + m_Camera.Dir, m_Camera.Up);
		Mat4 proj = Mat4Perspective(PI / 2, m_Camera.Aspect, 0.1f, 100.0f);
		m_Uniforms.MVP = proj * view;

		tri[0].ModelPos = { -10.0f, 10.0f, -10.0f, 1.0f };
		tri[1].ModelPos = { -10.0f, -10.0f, -10.0f, 1.0f };
		tri[2].ModelPos = { 10.0f, -10.0f, -10.0f, 1.0f };
		Renderer::Draw(m_Framebuffer, m_Program, tri, m_Uniforms);

		m_Window->DrawFramebuffer(m_Framebuffer);
	}

}

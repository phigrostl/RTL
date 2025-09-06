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

	void Application::OnUpdate(float time) {
		m_Framebuffer.Clear();
		m_Window->DrawFramebuffer(m_Framebuffer);
	}

}

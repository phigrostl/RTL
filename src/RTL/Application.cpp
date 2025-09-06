#include "Application.h"

#include "../Window/Window.h"

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
	}

	void Application::Terminate() {
	}

	void Application::Run() {
		bool running = true;
		while (running) {
			OnUpdate();
		}
	}

	void Application::OnUpdate() {
	}

}

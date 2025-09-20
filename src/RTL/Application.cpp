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
	}

	void Application::Terminate() {
		delete m_Window;
		Window::Terminate();
	}

	void Application::Run() {
		while (!m_Window->Closed()) {
			OnUpdate();
		}
	}

	void Application::OnUpdate() {

	}

}

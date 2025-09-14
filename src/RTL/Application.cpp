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
		m_Running = true;
	}

	void Application::Terminate() {

	}

	void Application::Run() {
		while (m_Running) {
			OnUpdate();
		}
	}

	void Application::OnUpdate() {

	}

}

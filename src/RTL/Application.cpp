#include "Application.h"

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
	}

	void Application::Terminate() {
	}

	void Application::Run() {
	}

	void Application::OnUpdate() {
	}

}

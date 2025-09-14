#pragma once

#include <string>

namespace RTL {

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

		void OnUpdate();

	private:
		std::string m_Name;
		int m_Width, m_Height;

		bool m_Running;
	};

}

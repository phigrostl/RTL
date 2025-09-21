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
		m_Camera.Aspect = (float)m_Width / (float)m_Height;
		m_Program.EnableDoubleSided = true;
		LoadMesh("../../../assets/DepthTest.obj");
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

		Mat4 view = Mat4LookAt(m_Camera.Pos, m_Camera.Pos + m_Camera.Dir, m_Camera.Up);
		Mat4 proj = Mat4Perspective(PI / 2.0f, m_Camera.Aspect, 0.1f, 100.0f);

		Mat4 model = Mat4Identity();
		m_Uniforms.MVP = proj * view * model;

		for (int i = 0; i < m_Mesh.size(); i++) {
			Renderer::Draw(m_Framebuffer, m_Program, m_Mesh[i], m_Uniforms);
		}
		
	}

	void Application::LoadMesh(const char* fileName) {
		std::ifstream file(fileName);

		std::vector<Vec3> positions;
		std::vector<Vec2> texCoords;
		std::vector<Vec3> normals;
		std::vector<size_t> posIndices;
		std::vector<size_t> texIndices;
		std::vector<size_t> normIndices;

		std::string line;
		while (!file.eof()) {
			std::getline(file, line);
			int items = -1;
			if (line.find("v ") == 0) {
				Vec3 position;
				items = sscanf(line.c_str(), "v %f %f %f",
					&position.X, &position.Y, &position.Z);
				ASSERT(items == 3);
				positions.push_back(position);
			}
			else if (line.find("vt ") == 0) {
				Vec3 texCoord;
				items = sscanf(line.c_str(), "vt %f %f",
					&texCoord.X, &texCoord.Y);
				ASSERT(items == 2);
				texCoords.push_back(texCoord);
			}
			else if (line.find("vn ") == 0) {
				Vec3 normal;
				items = sscanf(line.c_str(), "vn %f %f %f",
					&normal.X, &normal.Y, &normal.Z);
				ASSERT(items == 3);
				normals.push_back(normal);
			}
			else if (line.find("f ") == 0) {
				std::vector<size_t> numbers = GetNumbersFromString(line);
				std::vector<size_t> vertexIndices;
				std::vector<size_t> texCoordIndices;
				std::vector<size_t> normalIndices;
				for (size_t i = 0; i < numbers.size(); i += 3) {
					vertexIndices.push_back((numbers[i] - 1));
					texCoordIndices.push_back((numbers[i + 1] - 1));
					normalIndices.push_back((numbers[i + 2] - 1));
				}
				for (size_t i = 0; i < vertexIndices.size(); i++) {
					if (i < 3) {
						posIndices.push_back((vertexIndices[i]));
						texIndices.push_back((texCoordIndices[i]));
						normIndices.push_back((normalIndices[i]));
					}
					else {
						posIndices.push_back((vertexIndices[0]));
						posIndices.push_back((vertexIndices[i - 1]));
						posIndices.push_back((vertexIndices[i]));
						texIndices.push_back((texCoordIndices[0]));
						texIndices.push_back((texCoordIndices[i - 1]));
						texIndices.push_back((texCoordIndices[i]));
						normIndices.push_back((normalIndices[0]));
						normIndices.push_back((normalIndices[i - 1]));
						normIndices.push_back((normalIndices[i]));
					}
				}
			}
		}
		file.close();

		size_t triNum = posIndices.size() / 3;
		for (size_t i = 0; i < triNum; i++) {
			Triangle<BlinnVertex> triangle;
			for (int j = 0; j < 3; j++) {
				size_t index = i * 3 + j;
				size_t posIndex = posIndices[index];
				size_t texIndex = texIndices[index];
				size_t normIndex = normIndices[index];
				triangle[j].ModelPos = { positions[posIndex], 1 };
				triangle[j].TexCoord = texCoords[texIndex];
				triangle[j].ModelNormal = normals[normIndex];
			}
			m_Mesh.emplace_back(triangle);
		}

	}

}

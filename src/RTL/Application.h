#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "RTL/Window/Window.h"
#include "RTL/Renderer/Renderer.h"

#include <chrono>
#include <string>
#include <fstream>
#include <thread>
#include <direct.h>

namespace RTL {

	struct Camera {
		Vec4 Pos = Vec4(0.0f, 0.0f, -5.0f, 1.0f);
		Vec4 Right = Vec4(-1.0f, 0.0f, 0.0f, 0.0f);
		Vec4 Up = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
		Vec4 Dir = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
		Vec3 Rot = Vec3(0, 0, 0);
		float Aspect = 4.0f / 4.0f;
		float Fov = PI / 4.0f;
		float Near = 1.0f;
		float Far = 10.0f;
	};

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	class Application {
		using shader_t = void(*)(uniforms_t&);
		using vertex_shader_t = void (*)(varyings_t&, const vertex_t&, const uniforms_t&);
		using fragment_shader_t = Vec4(*)(bool& discard, const varyings_t&, const uniforms_t&);
	public:
		Application(
			int argc, char* argv[],
			const std::string& name,
			const int width, const int height,
			vertex_shader_t vertexShader, fragment_shader_t fragmentShader,
			shader_t shaderInit, shader_t shaderUpdate);

		~Application();

		void Run();

	private:
		void Init();
		void Terminate();

		void OnCameraUpdate(float time);
		void OnUpdate(float time);

		void RotateCamera(Camera& camera, Vec3 Ang);

		void LoadMesh(const char* fileName);
		void DrawTrianglesThreaded();

	private:
		std::string m_Name;
		int m_Width, m_Height;
		std::chrono::steady_clock::time_point m_LastFrameTime;

		Window* m_Window;
		Framebuffer* m_Framebuffer;

		Camera m_Camera;
		std::vector<Triangle<vertex_t>> m_Mesh;

		uniforms_t m_Uniforms;
		Program<vertex_t, varyings_t, uniforms_t> m_Program;

		shader_t m_ShaderInit;
		shader_t m_ShaderUpdate;

	private:
		bool isPress = false;
		Vec2 oriL;
	};

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	Application<vertex_t, varyings_t, uniforms_t>::Application(
		int argc, char* argv[],
		const std::string& name,
		const int width, const int height,
		vertex_shader_t vertexShader, fragment_shader_t fragmentShader,
		shader_t shaderInit, shader_t shaderUpdate)
		: m_Name(name), m_Width(width), m_Height(height),
		m_Program(vertexShader, fragmentShader),
		m_ShaderInit(shaderInit), m_ShaderUpdate(shaderUpdate) {

		Init();
	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	Application<vertex_t, varyings_t, uniforms_t>::~Application() {
		Terminate();
	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	void Application<vertex_t, varyings_t, uniforms_t>::Init() {

		if (_chdir("../../assets/"))
			exit(1);

		Window::Init();
		m_Window = Window::Create(m_Name, m_Width, m_Height);

		m_Framebuffer = Framebuffer::Create(m_Width, m_Height);
		m_Framebuffer->LoadFontTTF("simhei");

		m_Camera.Aspect = (float)m_Width / (float)m_Height;

		m_ShaderInit(m_Uniforms);

		LoadMesh("box.obj");
	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	void Application<vertex_t, varyings_t, uniforms_t>::Terminate() {
		delete m_Window;
		Window::Terminate();
	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	void Application<vertex_t, varyings_t, uniforms_t>::Run() {
		while (!m_Window->Closed()) {
			m_Framebuffer->Clear(Vec3(0.09f, 0.10f, 0.14f));
			m_Framebuffer->ClearDepth(m_Camera.Far);
			m_Window->PollInputEvents();

			float deltaTime = (std::chrono::steady_clock::now() - m_LastFrameTime).count() * 0.001f * 0.001f;

			m_LastFrameTime = std::chrono::steady_clock::now();

			OnUpdate(deltaTime);
			m_Window->DrawFramebuffer(m_Framebuffer);
		}
	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	void Application<vertex_t, varyings_t, uniforms_t>::OnCameraUpdate(float time) {
		constexpr float speed = 0.001f;
		Vec3 NorDir = Normalize({ m_Camera.Dir.X, 0.0f, m_Camera.Dir.Z });
		Vec3 NorRight = Normalize({ m_Camera.Right.X, 0.0f, m_Camera.Right.Z });
		if (m_Window->GetKey(RTL_KEY_W) == RTL_PRESS)
			m_Camera.Pos = m_Camera.Pos + Vec4(NorDir, m_Camera.Dir.W) * time * speed;
		if (m_Window->GetKey(RTL_KEY_S) == RTL_PRESS)
			m_Camera.Pos = m_Camera.Pos - Vec4(NorDir, m_Camera.Dir.W) * time * speed;
		if (m_Window->GetKey(RTL_KEY_A) == RTL_PRESS)
			m_Camera.Pos = m_Camera.Pos - Vec4(NorRight, m_Camera.Right.W) * time * speed;
		if (m_Window->GetKey(RTL_KEY_D) == RTL_PRESS)
			m_Camera.Pos = m_Camera.Pos + Vec4(NorRight, m_Camera.Right.W) * time * speed;
		if (m_Window->GetKey(RTL_KEY_SPACE) == RTL_PRESS)
			m_Camera.Pos.Y += time * speed;
		if (m_Window->GetKey(RTL_KEY_LEFT_SHIFT) == RTL_PRESS)
			m_Camera.Pos.Y -= time * speed;

		if (m_Window->GetKey(RTL_KEY_Z) == RTL_PRESS)
			m_Camera.Fov = Clamp(m_Camera.Fov + speed * time * 0.5f, 0, PI * 2);
		if (m_Window->GetKey(RTL_KEY_X) == RTL_PRESS)
			m_Camera.Fov = Clamp(m_Camera.Fov - speed * time * 0.5f, 0, PI * 2);

		if (!isPress && m_Window->GetKey(RTL_BUTTON_LEFT) == RTL_PRESS) {
			oriL = Vec2((float)m_Window->GetMouseX(), (float)m_Window->GetMouseY());
			isPress = true;
		}
		else if (m_Window->GetKey(RTL_BUTTON_LEFT) == RTL_PRESS && isPress) {
			Vec2 nowL = Vec2((float)m_Window->GetMouseX(), (float)m_Window->GetMouseY());
			Vec2 delta = nowL - oriL;
			float ax = delta.X / m_Width * PI * 2;
			float ay = delta.Y / m_Height * PI;
			float rotateSpeed = 0.5;
			RotateCamera(m_Camera, Vec3(-ay, -ax, 0.0f));
			oriL = nowL;
		}
		else if (isPress) {
			isPress = false;
		}
	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	void Application<vertex_t, varyings_t, uniforms_t>::RotateCamera(Camera& camera, Vec3 Ang) {

		Mat4 rotation = Mat4Identity();
		if (Ang.Y != 0.0f)
			rotation = Mat4RotateY(Ang.Y) * rotation;

		if (Ang.X != 0.0f)
			rotation = Mat4RotateAxis(camera.Right, Ang.X) * rotation;

		if (Ang.Z != 0.0f)
			rotation = Mat4RotateAxis(camera.Dir, Ang.Z) * rotation;

		camera.Dir = rotation * camera.Dir;
		camera.Right = rotation * camera.Right;
		camera.Right = { Normalize(camera.Right), 0.0f };
	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	void Application<vertex_t, varyings_t, uniforms_t>::DrawTrianglesThreaded() {
		size_t threadCount = std::thread::hardware_concurrency();
		threadCount = std::max<size_t>(threadCount, (size_t)1);

		const size_t triangleCount = m_Mesh.size();
		if (triangleCount == 0) return;

		const size_t trianglePerThread = triangleCount / threadCount;
		const size_t remainingTriangles = triangleCount % threadCount;

		std::vector<std::thread> threads;
		threads.reserve(threadCount);

		size_t currentTriangle = 0;

		for (size_t i = 0; i < threadCount; i++) {
			size_t threadTriangleCount = trianglePerThread + (i < remainingTriangles ? 1 : 0);
			size_t threadTriangleStart = currentTriangle;
			size_t threadTriangleEnd = currentTriangle + threadTriangleCount;
			currentTriangle = threadTriangleEnd;

			threads.emplace_back([&, threadTriangleStart, threadTriangleEnd]() {
				for (size_t j = threadTriangleStart; j < threadTriangleEnd; j++) {
					Renderer::Draw(m_Framebuffer, m_Program, m_Mesh[j], m_Uniforms);
				}
			});
		}

		for (auto& thread : threads) {
			if (thread.joinable())
				thread.join();
		}
	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	void Application<vertex_t, varyings_t, uniforms_t>::OnUpdate(float time) {
		OnCameraUpdate(time);

		Mat4 view = Mat4LookAt(m_Camera.Pos, m_Camera.Pos + m_Camera.Dir, m_Camera.Up);
		Mat4 proj = Mat4Perspective(m_Camera.Fov, m_Camera.Aspect, m_Camera.Near, m_Camera.Far);

		Mat4 model = Mat4Identity();
		m_Uniforms.MVP = proj * view * model;
		m_Uniforms.CameraPos = m_Camera.Pos;
		m_Uniforms.Model = model;
		
		m_ShaderUpdate(m_Uniforms);

		DrawTrianglesThreaded();

		float FPS = 1.0f / time * 1000.0f;
        m_Framebuffer->DrawTextTTF(0, 0, std::to_string(FPS), Vec3(1.0f, 1.0f, 1.0f), 20.0f);

	}

	template<typename vertex_t, typename varyings_t, typename uniforms_t>
	void Application<vertex_t, varyings_t, uniforms_t>::LoadMesh(const char* fileName) {
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
			Triangle<vertex_t> triangle;
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

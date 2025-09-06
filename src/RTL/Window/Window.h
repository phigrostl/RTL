#pragma once

#include "Framebuffer.h"
#include "InputCodes.h"

#include <string>
#include <Windows.h>

namespace RTL {
	class Window {
	public:
		Window(const std::string title, const int width, const int height);
		~Window();

		void Show();

		bool Closed() const { return m_Closed; }

		char GetKey(const uint32_t index) const { return m_Keys[index]; }

		void DrawFramebuffer(const Framebuffer& framebuffer);

	public:
		static void Init();
		static void Terminate();
		static Window* Create(const std::string name, const int width, const int height);

		static void Register();
		static void Unregister();

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam);

		static void PollInputEvents();

		static void KeyPressImpl(Window* window, const WPARAM wParam, const char state);

	protected:
		std::string m_Title;
		int m_Width, m_Height;
		bool m_Closed = true;
		HWND m_Handle;
		HDC m_MemoryDC;
		unsigned char* m_Buffer;

		static bool s_Inited;

		char m_Keys[RTL_KEY_MAX_COUNT];
	};
}

#pragma once

#include "RTL/Base/Base.h"

#include <string>
#include <Windows.h>

namespace RTL {

	class Window {
	public:
		Window(const std::string title, int width, int height);
		~Window();

		static void Init();
		static void Terminate();
		static Window* Create(const std::string title, int width, int height);

		void Show();

		bool Closed() const { return m_Closed; }

		static void Register();
		static void Unregister();

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam);

	protected:
		std::string m_Title;
		int m_Width, m_Height;
		bool m_Closed = true;

		HWND m_Handle;
		HDC m_MemoryDC;
		unsigned char* m_Buffer;

		static bool s_Inited;
	};

}

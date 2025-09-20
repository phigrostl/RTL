#include "Window.h"

#define RTL_WINDOW_ENTRY_NAME "Entry"
#define RTL_WINDOW_CLASS_NAME "Class"

namespace RTL {

	bool Window::s_Inited = false;

	Window::Window(const std::string title, int width, int height)
		: m_Title(title), m_Width(width), m_Height(height) {
		DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.bottom = (long)height;
		rect.right = (long)width;
		AdjustWindowRect(&rect, style, false);
		m_Handle = CreateWindow(RTL_WINDOW_CLASS_NAME, m_Title.c_str(), style,
								CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top,
								NULL, NULL, GetModuleHandle(NULL), NULL);
		m_Closed = false;
		SetProp(m_Handle, RTL_WINDOW_ENTRY_NAME, this);

		HDC windowDC = GetDC(m_Handle);
		m_MemoryDC = CreateCompatibleDC(windowDC);

		BITMAPINFOHEADER biHeader = {};
		HBITMAP newBitmap;
		HBITMAP oldBitmap;

		biHeader.biSize = sizeof(BITMAPINFOHEADER);
		biHeader.biWidth = ((long)m_Width);
		biHeader.biHeight = -((long)m_Height);
		biHeader.biPlanes = 1;
		biHeader.biBitCount = 24;
		biHeader.biCompression = BI_RGB;

		newBitmap = CreateDIBSection(m_MemoryDC, (BITMAPINFO*)&biHeader, DIB_RGB_COLORS, (void**)&m_Buffer, nullptr, 0);
		ASSERT(newBitmap != NULL);
		constexpr int channelCount = 3;
		int size = m_Width * m_Height * channelCount * sizeof(unsigned char);
		memset(m_Buffer, 0, size);
		oldBitmap = (HBITMAP)SelectObject(m_MemoryDC, newBitmap);

		DeleteObject(oldBitmap);
		ReleaseDC(m_Handle, windowDC);

		Show();

		memset(m_Keys, RTL_RELEASE, RTL_KEY_MAX_COUNT);
	}

	Window::~Window() {
		ShowWindow(m_Handle, SW_HIDE);
		RemoveProp(m_Handle, RTL_WINDOW_ENTRY_NAME);
		DeleteDC(m_MemoryDC);
		DestroyWindow(m_Handle);
	}

	void Window::Init() {
		Register();
		s_Inited = true;
	}

	void Window::Terminate() {
		Unregister();
		s_Inited = false;
	}

	void Window::Register() {
		ATOM atom;
		WNDCLASS wc = { 0 };
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = (HBRUSH)(WHITE_BRUSH);
		wc.hCursor = NULL;
		wc.hIcon = NULL;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = WndProc;
		wc.lpszClassName = RTL_WINDOW_CLASS_NAME;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpszMenuName = NULL;
		atom = RegisterClass(&wc);
	}

	void Window::Unregister() {
		UnregisterClass(RTL_WINDOW_CLASS_NAME, GetModuleHandle(NULL));
	}

	void Window::Show() {
		HDC windowDC = GetDC(m_Handle);
		BitBlt(windowDC, 0, 0, m_Width, m_Height, m_MemoryDC, 0, 0, SRCCOPY);
		ShowWindow(m_Handle, SW_SHOW);
		ReleaseDC(m_Handle, windowDC);
	}

	LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam) {
		Window* window = (Window*)GetProp(hWnd, RTL_WINDOW_ENTRY_NAME);
		if (window == nullptr)
			return DefWindowProc(hWnd, msgID, wParam, lParam);

		switch (msgID) {
			case WM_DESTROY:
				window->m_Closed = true;
				return 0;
			case WM_KEYDOWN:
				window->m_Keys[wParam] = RTL_PRESS;
				return 0;
			case WM_KEYUP:
				window->m_Keys[wParam] = RTL_RELEASE;
				return 0;
		}
		return DefWindowProc(hWnd, msgID, wParam, lParam);
	}

	void Window::PollInputEvents() {
		MSG message;
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	Window* Window::Create(const std::string title, int width, int height) {
		return new Window(title, width, height);
	}

}

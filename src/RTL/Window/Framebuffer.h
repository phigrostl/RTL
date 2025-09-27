#pragma once

#include "RTL/Base/Maths.h"
#include <stb_image/stb_truetype.h>
#include <fstream>

namespace RTL {

	class Framebuffer {
	public:
		Framebuffer(const int width, const int height);
		~Framebuffer();

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		void SetColor(const int x, const int y, const Vec3& color);
		Vec3 GetColor(const int x, const int y) const;
		void SetDepth(const int x, const int y, const float depth);
		float GetDepth(const int x, const int y) const;

		void Clear(const Vec3& color = Vec3(0.0f, 0.0f, 0.0f));
		void ClearDepth(const float depth = 1.0f);

		void LoadFontTTF(const std::string& fontPath);
		// short
		void DrawCharTTF(int x, int y, char c, const Vec3& color, float fontSize);
		void DrawTextTTF(int x, int y, const std::string& text, const Vec3& color, float fontSize);

		// wide
		void DrawWCharTTF(int x, int y, wchar_t c, const Vec3& color, float fontSize);
		void DrawWTextTTF(int x, int y, const std::wstring& text, const Vec3& color, float fontSize);

		static Framebuffer* Create(const int width, const int height);

	private:
		int GetPixelIndex(const int x, const int y) const { return (y * m_Width + x) * 3; }

	private:
		int m_Width;
		int m_Height;
		int m_PixelSize;
		float* m_DepthBuffer;
		Vec3* m_ColorBuffer;

		stbtt_fontinfo m_FontInfo;
		std::vector<unsigned char> m_fontBuffer;
	};

}

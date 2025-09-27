#include "Framebuffer.h"

namespace RTL {

	Framebuffer::Framebuffer(const int width, const int height)
		: m_Width(width), m_Height(height) {
		ASSERT(width > 0 && height > 0);
		m_PixelSize = m_Width * m_Height;
		m_ColorBuffer = new Vec3[m_PixelSize]();
		m_DepthBuffer = new float[m_PixelSize]();
		Clear();
		ClearDepth();
	}

	Framebuffer::~Framebuffer() {
		delete[] m_ColorBuffer;
		delete[] m_DepthBuffer;
		m_ColorBuffer = nullptr;
		m_DepthBuffer = nullptr;
	}

	void Framebuffer::SetColor(const int x, const int y, const Vec3& color) {
		if (x >= 0 && x < m_Width && y >= 0 && y < m_Height)
			m_ColorBuffer[x + y * m_Width] = color;
		else
			ASSERT(false);
	}

	Vec3 Framebuffer::GetColor(const int x, const int y) const {
		if (x >= 0 && x < m_Width && y >= 0 && y < m_Height)
			return m_ColorBuffer[x + y * m_Width];
		else
			ASSERT(false);
		return Vec3(0.0f, 0.0f, 0.0f);
	}

	void Framebuffer::SetDepth(const int x, const int y, const float depth) {
		if (x >= 0 && x < m_Width && y >= 0 && y < m_Height)
			m_DepthBuffer[x + y * m_Width] = depth;
		else
            ASSERT(false);
	}

	float Framebuffer::GetDepth(const int x, const int y) const {
		if (x >= 0 && x < m_Width && y >= 0 && y < m_Height)
			return m_DepthBuffer[x + y * m_Width];
		else
			ASSERT(false);
		return 1.0f;
	}

	void Framebuffer::Clear(const Vec3& color) {
		for (int i = 0; i < m_PixelSize; i++)
			m_ColorBuffer[i] = color;
	}

	void Framebuffer::ClearDepth(const float depth) {
		for (int i = 0; i < m_PixelSize; i++)
			m_DepthBuffer[i] = depth;
	}

	void Framebuffer::LoadFontTTF(const std::string& fontPath) {
		std::ifstream file(fontPath, std::ios::binary);
		if (!file) return;
		m_fontBuffer = std::vector<unsigned char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
        stbtt_InitFont(&m_FontInfo, m_fontBuffer.data(), 0);
	}

	// short
	void Framebuffer::DrawCharTTF(int x, int y, char c, const Vec3& color, float fontSize) {
		unsigned char* bitmap;
		int w, h, xoff, yoff;
		float scale = stbtt_ScaleForPixelHeight(&m_FontInfo, fontSize);

		bitmap = stbtt_GetCodepointBitmap(&m_FontInfo, 0, scale, c, &w, &h, &xoff, &yoff);

		int ascent, descent, lineGap;;
		stbtt_GetFontVMetrics(&m_FontInfo, &ascent, &descent, &lineGap);
		int baseline = int(ascent * scale);

		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				float alpha = bitmap[i + j * w] / 255.0f;
				if (alpha > 0.1f) {
					Vec3 col = color * alpha;
					SetColor(x + i + xoff, m_Height - (y + j + yoff + baseline), col);
				}
			}
		}
		stbtt_FreeBitmap(bitmap, nullptr);
	}

	void Framebuffer::DrawTextTTF(int x, int y, const std::string& text, const Vec3& color, float fontSize) {
		float scale = stbtt_ScaleForPixelHeight(&m_FontInfo, fontSize);
		int xpos = x;
		
		for (char c : text) {
			int ax;
			int lsb;
			stbtt_GetCodepointHMetrics(&m_FontInfo, c, &ax, &lsb);
			DrawCharTTF(xpos, y, c, color, fontSize);
			int kern = stbtt_GetCodepointKernAdvance(&m_FontInfo, c, c);
			xpos += int(ax * scale) + kern;
		}
	}

	// wide
	void Framebuffer::DrawWCharTTF(int x, int y, wchar_t c, const Vec3& color, float fontSize) {
		int w, h, xoff, yoff;
		float scale = stbtt_ScaleForPixelHeight(&m_FontInfo, fontSize);

		unsigned char* bitmap = stbtt_GetCodepointBitmap(
			&m_FontInfo, 0, scale, c, &w, &h, &xoff, &yoff);

		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				float alpha = bitmap[i + j * w] / 255.0f;
				if (alpha > 0.1f) {
					int px = x + i + xoff;
					int py = m_Height - (y + j + yoff);
					if (px >= 0 && px < m_Width && py >= 0 && py < m_Height) {
						Vec3 dst = GetColor(px, py);
						Vec3 blended = color * alpha + dst * (1.0f - alpha);
						SetColor(px, py, blended);
					}
				}
			}
		}
		stbtt_FreeBitmap(bitmap, nullptr);
	}

	void Framebuffer::DrawWTextTTF(int x, int y, const std::wstring& text, const Vec3& color, float fontSize) {
		float scale = stbtt_ScaleForPixelHeight(&m_FontInfo, fontSize);
		int xpos = x;
		int ascent, descent, lineGap;;
		stbtt_GetFontVMetrics(&m_FontInfo, &ascent, &descent, &lineGap);
		int baseline = int(ascent * scale);
		for (wchar_t c : text) {
			int ax;
			int lsb;
			stbtt_GetCodepointHMetrics(&m_FontInfo, c, &ax, &lsb);
			DrawWCharTTF(xpos, y + baseline, c, color, fontSize);
			int kern = stbtt_GetCodepointKernAdvance(&m_FontInfo, c, c);
			xpos += int(ax * scale) + kern;
		}
	}

	Framebuffer* Framebuffer::Create(const int width, const int height) {
		return new Framebuffer(width, height);
	}

}

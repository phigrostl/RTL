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

	Framebuffer* Framebuffer::Create(const int width, const int height) {
		return new Framebuffer(width, height);
	}

}

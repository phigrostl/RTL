#pragma once

#include "RTL/Base/Maths.h"

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

		static Framebuffer* Create(const int width, const int height);

	private:
		int GetPixelIndex(const int x, const int y) const { return (y * m_Width + x) * 3; }

	private:
		int m_Width;
		int m_Height;
		int m_PixelSize;
		float* m_DepthBuffer;
		Vec3* m_ColorBuffer;
	};

}

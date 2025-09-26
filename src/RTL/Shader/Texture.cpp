#include "Texture.h"

namespace RTL {

	Texture::Texture(const std::string& path)
		: m_Path(path) {
		Init();
	}

	Texture::~Texture() {
		if (m_Data)
			delete[] m_Data;
		m_Data = nullptr;
	}

	void Texture::Init() {
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);
		ASSERT(data);

		m_Height = height;
		m_Width = width;
		m_Channels = channels;
		int size = width * height;
		m_Data = new Vec4[size];

		switch (channels) {
		case 4:
			for (int i = 0; i < size; i++) {
				m_Data[i] = Vec4(
					UChar2Float(data[i * 4]),
					UChar2Float(data[i * 4 + 1]),
					UChar2Float(data[i * 4 + 2]),
					UChar2Float(data[i * 4 + 3])
				);
			}
			break;

		case 3:
			for (int i = 0; i < size; i++) {
				m_Data[i] = Vec4(
					UChar2Float(data[i * 3]),
					UChar2Float(data[i * 3 + 1]),
					UChar2Float(data[i * 3 + 2]),
					0.0f
				);
			}
			break;

		case 2:
			for (int i = 0; i < size; i++) {
				m_Data[i] = Vec4(
					UChar2Float(data[i * 2]),
					UChar2Float(data[i * 2 + 1]),
					0.0f,
					0.0f
				);
			}
			break;

		case 1:
			for (int i = 0; i < size; i++) {
				m_Data[i] = Vec4(
					UChar2Float(data[i]),
					0.0f,
					0.0f,
					0.0f
				);
			}
			break;

		default:
            ASSERT(false);
			break;
		}
	}

	Vec4 Texture::Sample(Vec2 texCoords) const {
		float vx = Clamp(texCoords.X, 0.0f, 1.0f);
		float vy = Clamp(texCoords.Y, 0.0f, 1.0f);

		float fx = vx * (m_Width - 1);
		float fy = vy * (m_Height - 1);

		int x0 = (int)fx;
		int y0 = (int)fy;
		int x1 = (int)Clamp((float)x0 + 1.0f, 0.0f, (float)m_Width - 1.0f);
		int y1 = (int)Clamp((float)y0 + 1.0f, 0.0f, (float)m_Height - 1.0f);

		float dx = fx - x0;
		float dy = fy - y0;

		Vec4 c00 = m_Data[x0 + y0 * m_Width];
		Vec4 c10 = m_Data[x1 + y0 * m_Width];
		Vec4 c01 = m_Data[x0 + y1 * m_Width];
		Vec4 c11 = m_Data[x1 + y1 * m_Width];

		Vec4 c0 = c00 * (1 - dx) + c10 * dx;
		Vec4 c1 = c01 * (1 - dx) + c11 * dx;
		Vec4 c = c0 * (1 - dy) + c1 * dy;

		return c;
	}

}

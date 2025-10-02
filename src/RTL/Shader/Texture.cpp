#include "Texture.h"

namespace RTL {

	Texture::Texture(const std::string& path)
		: m_Path(path) {
		Init();
	}

	Texture::Texture(const float value) {
		m_Width = 1;
		m_Height = 1;
		m_Channels = 4;
		m_Data = new Vec4[1];
		m_Data[0] = Vec4(value, value, value, value);
	}

	Texture::Texture(const Vec4& value) {
		m_Width = 1;
		m_Height = 1;
		m_Channels = 4;
		m_Data = new Vec4[1];
		m_Data[0] = value;
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

	Vec4 Texture::Sample(Vec2 texCoords, bool enableLerp, Vec4 defaultValue) const {
		if (this == nullptr)
			return defaultValue;
		if (m_Data == nullptr)
			return defaultValue;
		if (!enableLerp) {
			float vx = Clamp(texCoords.X, 0.0f, 1.0f);
			float vy = Clamp(texCoords.Y, 0.0f, 1.0f);

			int x = (int)(vx * (m_Width - 1) + 0.5f);
			int y = (int)(vy * (m_Height - 1) + 0.5f);

			int index = x + y * m_Width;
			return m_Data[index];
		}
		else {
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

	float Texture::SampleFloat(Vec2 texCoords, bool enableLerp, float defaultValue) const {
		if (this == nullptr)
			return defaultValue;
		Vec4 c;
		switch (m_Channels) {
		case 1:
			return Sample(texCoords, enableLerp, Vec4(defaultValue, 0.0f, 0.0f, 0.0f)).X;
		case 2:
			c = Sample(texCoords, enableLerp, Vec4(defaultValue, defaultValue, 0.0f, 0.0f));
			return (c.X + c.Y) / 2.0f;
		case 3:
			c = Sample(texCoords, enableLerp, Vec4(defaultValue, defaultValue, defaultValue, 0.0f));
			return (c.X + c.Y + c.Z) / 3.0f;
		case 4:
			c = Sample(texCoords, enableLerp, Vec4(defaultValue, defaultValue, defaultValue, defaultValue));
			return (c.X + c.Y + c.Z + c.W) / 4.0f;
		default:
			return defaultValue;
		}
	}


	Vec3 TextureSphere::Sample(const Vec3& v3) const {
		Vec3 dir = Normalize(v3);
		float phi = atan2(dir.Z, dir.X);
		float theta = acos(dir.Y);
		float u = phi / (2 * PI) + 0.5f;
		float v = 1.0f - theta / PI;

		int x = (int)(u * (m_Width - 1) + 0.5f);
		int y = (int)(v * (m_Height - 1) + 0.5f);

		int index = x + y * m_Width;
		return m_Data[index];
	}

	TextureSphere::TextureSphere(const std::string& path)
		:m_Path(path) {
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		float* data;
		data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
		ASSERT((data) && (width > 0) && (height > 0) && (channels == 3));

		m_Height = height;
		m_Width = width;
		m_Channels = channels;
		m_PixelSize = height * width;

		int size = height * width;
		m_Data = new Vec3[size];
		memcpy(m_Data, data, size * sizeof(Vec3));
		stbi_image_free(data);
	}

	TextureSphere::TextureSphere(const Framebuffer& framebuffer) {
		m_Width = framebuffer.GetWidth();
		m_Height = framebuffer.GetHeight();
		m_Channels = 3;
		int size = m_Width * m_Height;
		m_PixelSize = size;
		m_Data = new Vec3[size];
		memcpy(m_Data, framebuffer.GetRawColorData(), size * sizeof(Vec3));
	}

	TextureSphere::~TextureSphere() {
		delete[] m_Data;
		m_Data = nullptr;
	}

	TextureSphere* TextureSphere::LoadTextureSphere(const std::string& path) {
		int width, height, channels;
		float* data;
		stbi_set_flip_vertically_on_load(true);
		data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

		ASSERT(data == nullptr || width <= 0 || height <= 0 || channels != 3)

		TextureSphere* res = new TextureSphere();

		res->m_Height = height;
		res->m_Width = width;
		res->m_Channels = channels;
		res->m_PixelSize = height * width;

		int size = height * width;
		res->m_Data = new Vec3[size];
		memcpy(res->m_Data, data, size * sizeof(Vec3));
		stbi_image_free(data);
		return res;
	}

	LodTextureSphere::LodTextureSphere(std::vector<std::string> paths) {
		ASSERT(paths.size() == 5);

		stbi_set_flip_vertically_on_load(true);
		int width, height, channels, size;
		float* data;
		for (int i = 0; i < 5; i++) {
			data = stbi_loadf(paths[i].c_str(), &width, &height, &channels, 0);
			ASSERT((data) && (width > 0) && (height > 0) && (channels == 3));
			size = height * width;

			m_Data[i].Height = height;
			m_Data[i].Width = width;
			m_Data[i].Channels = channels;
			m_Data[i].PixelSize = size;
			m_Data[i].ColorData = new Vec3[size];
			memcpy(m_Data[i].ColorData, data, size * sizeof(Vec3));

			stbi_image_free(data);
		}
	}

	LodTextureSphere::LodTextureSphere(std::string path) {
		stbi_set_flip_vertically_on_load(true);
		int in_width, in_height, in_channels;
		float* in_data;

		in_data = stbi_loadf(path.c_str(), &in_width, &in_height, &in_channels, 0);
		ASSERT(((in_data) && (in_width >= 32 * 2) && (in_height >= 32) && (in_channels == 3)));

		int width = 2048;
		int height = width / 2;

		for (int i = 0; i < 5; ++i) {
			float* data = stbir_resize_float_linear(in_data, in_width, in_height, 0,
				nullptr, width, height, 0, stbir_pixel_layout::STBIR_RGB);
			int size = width * height;
			m_Data[i].ColorData = new Vec3[size];
			m_Data[i].Height = height;
			m_Data[i].Width = width;
			m_Data[i].Channels = 3;
			m_Data[i].PixelSize = size;
			memcpy(m_Data[i].ColorData, data, size * sizeof(Vec3));
			stbi_image_free(data);

			width /= 2;
			height /= 2;
		}

		stbi_image_free(in_data);
	}

	LodTextureSphere::~LodTextureSphere() {
		for (auto data : m_Data) {
			delete data.ColorData;
		}
	}

	LodTextureSphere* LodTextureSphere::LoadLodTextureSphere(const std::string& path, LoadType loadType) {
		if (loadType == LoadType::SingleFile)
			ASSERT(false);
		else if (loadType == LoadType::Directory) {
			stbi_set_flip_vertically_on_load(true);
			int width, height, channels, size;
			float* data;
			LodTextureSphere* res = new LodTextureSphere();
			res->m_Path = path;
			for (int i = 0; i < 5; i++) {
				std::string loadPath{ path };
				loadPath.append("\\");
				loadPath.append(std::to_string(i));
				loadPath.append(".hdr");

				data = stbi_loadf(loadPath.c_str(), &width, &height, &channels, 0);

				ASSERT((data) && (width > 0) && (height > 0) && (channels == 3));
				size = height * width;
				res->m_Data[i].Height = height;
				res->m_Data[i].Width = width;
				res->m_Data[i].Channels = channels;
				res->m_Data[i].PixelSize = size;
				res->m_Data[i].ColorData = new Vec3[size];
				memcpy(res->m_Data[i].ColorData, data, size * sizeof(Vec3));
				stbi_image_free(data);
			}
			return res;
		}
		ASSERT(false);
		return nullptr;
	}

	Vec3 LodTextureSphere::Sample(const Vec3& v3, float lod) const {
		ASSERT((lod >= 0) && (lod <= (float)4));
		int number = (int)floor(lod);
		float frac = fmod(lod, 1.0f);

		Vec3 dir = Normalize(v3);
		float phi = atan2(dir.Z, dir.X);
		float theta = acos(dir.Y);
		float u = phi / (2.0f * PI) + 0.5f;
		float v = 1.0f - theta / PI;

#if 1
		if (number == 4) {
			u = u * (m_Data[4].Width - 1) + 0.5f;
			v = v * (m_Data[4].Height - 1) + 0.5f;
			int x = (int)floor(u);
			int y = (int)floor(v);
			float fracX = fmod(u, 1.0f);
			float fracY = fmod(v, 1.0f);

			Vec3 res{ 0.0f, 0.0f, 0.0f };
			res += GetColor(x, y, 4) * (1.0f - fracX) * (1.0f - fracY);
			res += GetColor(x + 1, y, 4) * fracX * (1.0f - fracY);
			res += GetColor(x, y + 1, 4) * (1.0f - fracX) * fracY;
			res += GetColor(x + 1, y + 1, 4) * fracX * fracY;
			return res;
		}
		else {
			float u0 = u * (m_Data[number].Width - 1) + 0.5f;
			float v0 = v * (m_Data[number].Height - 1) + 0.5f;
			int x0 = (int)floor(u0);
			int y0 = (int)floor(v0);
			float fracX0 = fmod(u0, 1.0f);
			float fracY0 = fmod(v0, 1.0f);

			Vec3 c0{ 0.0f, 0.0f, 0.0f };
			c0 += GetColor(x0, y0, number) * (1.0f - fracX0) * (1.0f - fracY0);
			c0 += GetColor(x0 + 1, y0, number) * fracX0 * (1.0f - fracY0);
			c0 += GetColor(x0, y0 + 1, number) * (1.0f - fracX0) * fracY0;
			c0 += GetColor(x0 + 1, y0 + 1, number) * fracX0 * fracY0;

			float u1 = u * (m_Data[number + 1].Width - 1) + 0.5f;
			float v1 = v * (m_Data[number + 1].Height - 1) + 0.5f;
			int x1 = (int)floor(u1);
			int y1 = (int)floor(v1);
			float fracX1 = fmod(u1, 1.0f);
			float fracY1 = fmod(v1, 1.0f);

			Vec3 c1{ 0.0f, 0.0f, 0.0f };
			c1 += GetColor(x1, y1, number + 1) * (1.0f - fracX1) * (1.0f - fracY1);
			c1 += GetColor(x1 + 1, y1, number + 1) * fracX1 * (1.0f - fracY1);
			c1 += GetColor(x1, y1 + 1, number + 1) * (1.0f - fracX1) * fracY1;
			c1 += GetColor(x1 + 1, y1 + 1, number + 1) * fracX1 * fracY1;

			Vec3 color = Lerp(c0, c1, frac);
			return color;
		}
#else
		if (number == 4)
		{
			u = u * (m_Data[4].Width - 1) + 0.5f;
			v = v * (m_Data[4].Height - 1) + 0.5f;

			int x = floor(u);
			int y = floor(v);

			int index = (int)y * m_Data[4].Width + (int)x;
			return m_Data[4].ColorData[index];
		}
		else
		{
			int x0 = u * (m_Data[number].Width - 1) + 0.5f;
			int y0 = v * (m_Data[number].Height - 1) + 0.5f;
			int index0 = y0 * m_Data[number].Width + x0;

			int x1 = u * (m_Data[number + 1].Width - 1) + 0.5f;
			int y1 = v * (m_Data[number + 1].Height - 1) + 0.5f;
			int index1 = y1 * m_Data[number + 1].Width + x1;

			Vec3 c0 = m_Data[number].ColorData[index0];
			Vec3 c1 = m_Data[number + 1].ColorData[index1];
			Vec3 color = Lerp(c0, c1, frac);
			return color;
		}
#endif 

		return { 0.0f, 0.0f, 0.0f };
	}

}

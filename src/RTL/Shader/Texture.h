#pragma once
#include "RTL/Base/Maths.h"
#include "RTL/Window/Framebuffer.h"

#include <string>
#include <stb_image/stb_image.h>
#include <stb_image/stb_image_resize2.h>

namespace RTL {

	class Texture {
	public:
		Texture(const std::string& path);
		Texture(const float value);
		Texture(const Vec4& value);
		~Texture();

		Vec4 Sample(Vec2 texCoords, bool enableLerp = true, Vec4 defaultValue = Vec4(0.0f)) const;
		float SampleFloat(Vec2 texCoords, bool enableLerp = true, float defaultValue = 0.0f) const;

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		std::string GetPath() const { return m_Path; }

	private:
		void Init();

	private:
		int m_Width, m_Height, m_Channels;
		std::string m_Path;
		Vec4* m_Data;
	};

	class TextureSphere {
	public:
		TextureSphere(const std::string& path);
		TextureSphere(const Framebuffer& framebuffer);
		~TextureSphere();

		Vec3 Sample(const Vec3& v3) const;

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		std::string GetPath() const { return m_Path; }

		static TextureSphere* LoadTextureSphere(const std::string& path);

	protected:
		TextureSphere() = default;

	protected:
		int m_Width, m_Height, m_Channels, m_PixelSize;
		std::string m_Path;
		Vec3* m_Data;
	};

	class LodTextureSphere {
	public:
		LodTextureSphere(std::vector<std::string> paths);
		LodTextureSphere(std::string paths);
		~LodTextureSphere();
		Vec3 Sample(const Vec3& v3, float lod) const;

		Vec3 GetColor(int x, int y, int lod) const {
			const Data& data = m_Data[lod];
			int width = data.Width;
			int height = data.Height;
			x %= width;
			y %= height;
			return data.ColorData[y * width + x];
		}

		enum class LoadType {
			SingleFile,
			Directory,
		};

		static LodTextureSphere* LoadLodTextureSphere(const std::string& path, LoadType loadType);

		std::string GetPath() { return m_Path; }

	protected:
		LodTextureSphere() = default;

		std::string m_Path;

		struct Data {
			int Width, Height, Channels, PixelSize;
			Vec3* ColorData;
		};
		Data m_Data[5];
	};

}

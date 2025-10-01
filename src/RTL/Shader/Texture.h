#pragma once
#include "RTL/Base/Maths.h"

#include <string>
#include <stb_image/stb_image.h>

namespace RTL {

	class Texture {
	public:
		Texture(const std::string& path);
		~Texture();

		Vec4 Sample(Vec2 texCoords, bool enableLerp = true, Vec4 defaultValue = Vec4(0.0f)) const;

	private:
		void Init();

	private:
		int m_Width, m_Height, m_Channels;
		std::string m_Path;
		Vec4* m_Data;
	};

}

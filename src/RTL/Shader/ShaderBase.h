#pragma once
#include "RTL/Base/Maths.h"

namespace RTL {

	struct VertexBase {
		Vec4 ModelPos = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	struct VaryingsBase {
		Vec4 ClipPos = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 NdcPos = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 FragPos = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	struct UniformsBase {
		Mat4 MVP;
	};

}

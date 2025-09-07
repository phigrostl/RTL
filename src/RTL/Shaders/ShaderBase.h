#pragma once
#include "../Base/Maths.h"

namespace RTL {


	struct VertexBase {
		Vec4 ModelPos = Vec4(0, 0, 0, 1);
	};

	struct VaryingsBase {
		Vec4 ClipPos = Vec4(0, 0, 0, 1);
		Vec4 NdcPos = Vec4(0, 0, 0, 1);
		Vec4 FragPos = Vec4(0, 0, 0, 1);
	};

	struct UniformsBase {
		Mat4 MVP;
	};

}

#pragma once
#include "ShaderBase.h"
#include "../Base/Maths.h"

namespace RTL {

	struct BlinnVertex : public VertexBase {

	};

	struct BlinnVaryings : public VaryingsBase {

	};

	struct BlinnUniforms : public UniformsBase {

	};

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms);

}

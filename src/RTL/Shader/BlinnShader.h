#pragma once
#include "RTL/Base/Maths.h"
#include "RTL/Shader/ShaderBase.h"

namespace RTL {

	struct BlinnVertex : public VertexBase {

	};

	struct BlinnVaryings : public VaryingsBase {

	};

	struct BlinnUniforms : public UniformsBase {

	};

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms);
	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms);
}

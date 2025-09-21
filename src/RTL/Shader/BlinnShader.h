#pragma once
#include "RTL/Base/Maths.h"
#include "RTL/Shader/ShaderBase.h"

namespace RTL {

	struct BlinnVertex : public VertexBase {
		Vec3 ModelNormal;
		Vec2 TexCoord = Vec2(0.0f, 0.0f);
	};

	struct BlinnVaryings : public VaryingsBase {
		Vec3 WorldPos;
		Vec3 WorldNormal;
		Vec2 TexCoord;
	};

	struct BlinnUniforms : public UniformsBase {
	};

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms);
	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms);
}

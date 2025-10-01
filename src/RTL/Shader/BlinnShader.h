#pragma once
#include "ShaderBase.h"

#include "RTL/Base/Maths.h"
#include "RTL//Shader/Texture.h"

namespace RTL {

	struct BlinnLight {
		Vec3 Position = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 Diffuse = Vec3(0.5f, 0.5f, 0.5f);
		Vec3 Specular = Vec3(1.0f, 1.0f, 1.0f);
		float Strength = 1.0f;
	};

	struct BlinnVertex : public VertexBase {
		
	};

	struct BlinnVaryings : public VaryingsBase {
		Vec3 WorldPos;
		Vec3 WorldNormal;
	};

	struct BlinnUniforms : public UniformsBase {
		Mat4 ModelNormalWorld;
		Vec3 Ambient = Vec3(0.7f, 0.7f, 0.7f);
		std::vector<BlinnLight> Lights;

		float Shininess = 3.0f;

		Texture* Diffuse = nullptr;
		Texture* Specular = nullptr;

		bool EnableLerpTexture = true;
	};

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms);
	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms);

	void BlinnOnUpdate(BlinnUniforms& uniforms);
	void BlinnInit(BlinnUniforms& uniforms);
}

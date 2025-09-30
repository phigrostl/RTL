#pragma once
#include "ShaderBase.h"
#include "RTL/Shader/Texture.h"

namespace RTL {

	struct PBRLight {
		Vec3 Position;
		Vec3 Color;
	};

	struct PBRVertex : public VertexBase {
		Vec3 Normal = Vec3(1.0f, 0.0f, 0.0f);
	};

	struct PBRVaryings : public VaryingsBase {
		Vec3 WorldPos;
		Vec3 WorldNormal;
	};

	struct PBRUniforms : public UniformsBase {
		Mat4 Model;
		Mat4 ModelNormalWorld;

		Vec3 Albedo = Vec3(1.0f, 1.0f, 1.0f);
		float Metalic = 0.5f;
		float Roughness = 0.5f;
		float Ao = 1.0f;

		Texture* Diffuse = nullptr;
		bool EnableLerpTexture = true;

		std::vector<PBRLight> Lights;

		Vec3 CameraPos;
	};

	void PBRVertexShader(PBRVaryings& varyings, const PBRVertex& vertex, const PBRUniforms& uniforms);
    Vec4 PBRFragmentShader(bool& discard, const PBRVaryings& varyings, const PBRUniforms& uniforms);

	void PBROnUpdate(PBRUniforms& uniforms);
	void PBRInit(PBRUniforms& uniforms);

}

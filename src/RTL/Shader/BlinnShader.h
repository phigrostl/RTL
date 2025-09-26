#pragma once
#include "RTL/Base/Maths.h"
#include "RTL/Shader/ShaderBase.h"
#include "RTL//Shader/Texture.h"

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
		Mat4 Model;
		Mat4 ModelNormalWorld;
		Vec3 LightPos = Vec3(0.0f, 1.0f, 2.0f);
		Vec3 LightAmbient = Vec3(0.3f, 0.3f, 0.3f);
		Vec3 LightDiffuse = Vec3(0.5f, 0.5f, 0.5f);
		Vec3 LightSpecular = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 ObjectColor = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 CameraPos;

		float Shininess = 32.0f;

		Texture* Diffuse = nullptr;
		Texture* Specular = nullptr;
	};

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms);
	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms);
}

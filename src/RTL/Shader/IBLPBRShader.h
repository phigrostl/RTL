#pragma once
#include "ShaderBase.h"

#include "RTL/Shader/Texture.h"

namespace RTL {

	struct IBLPBRVertex : public VertexBase {

	};

	struct IBLPBRVaryings : public VaryingsBase {
		Vec3 TexPos = Vec3(0.0f);
		Vec3 WorldNormal;
		Vec3 WorldPos;
	};

	struct IBLPBRUniforms : public UniformsBase {
        Mat4 NormalMatrix;
        Mat4 ModelMatrix;
        Mat4 ModelNormalWorld;
        Vec3 CamPos;

        Vec3 Albedo = Vec3(1.0f);
        float Metallic = 0.0f;
        float Roughness = 0.0f;
        float Ao = 1.0f;

        Vec3 LightPos = Vec3(0.0f, 0.0f, -1.0f);
        Vec3 LightColor = Vec3(1.0f, 0.0f, 0.0f);

        TextureSphere* IrradianceMap;
        LodTextureSphere* PrefilterMap;
        Texture* BrdfLUT;
	};

    void IBLPBRVertexShader(IBLPBRVaryings& varyings, const IBLPBRVertex& vertex, const IBLPBRUniforms& uniforms);
    Vec4 IBLPBRFragmentShader(bool& discard, const IBLPBRVaryings& varyings, const IBLPBRUniforms& uniforms);

    void IBLPBROnUpdate(IBLPBRUniforms& uniforms);
    void IBLPBRInit(IBLPBRUniforms& uniforms);

}

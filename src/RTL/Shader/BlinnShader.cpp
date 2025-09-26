#include "BlinnShader.h"

namespace RTL {

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms) {
		varyings.ClipPos = uniforms.MVP * vertex.ModelPos;
		varyings.TexCoord = vertex.TexCoord;
		varyings.WorldPos = uniforms.Model * vertex.ModelPos;
		varyings.WorldNormal = uniforms.ModelNormalWorld * Vec4(vertex.ModelNormal, 1.0f);
	}

	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms) {
		discard = false;

		const Vec3& cameraPos = uniforms.CameraPos;
		const Vec3& lightPos = uniforms.LightPos;
		const Vec3& worldPos = varyings.WorldPos;
		Vec3 worldNormal = Normalize(varyings.WorldNormal);
		Vec3 viewDir = Normalize(cameraPos - worldPos);
		Vec3 lightDir = Normalize(lightPos - worldPos);
		Vec3 halfDir = Normalize(viewDir + lightDir);

		Vec3 ambient = uniforms.LightAmbient;
		Vec3 specularStrength = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 diffColor = Vec3(1.0f, 1.0f, 1.0f);

		const Vec2& texCoord = varyings.TexCoord;
		if (uniforms.Diffuse) {
			diffColor = uniforms.Diffuse->Sample(texCoord);
			ambient = ambient * diffColor;
		}
		if (uniforms.Specular)
			specularStrength = uniforms.Specular->Sample(texCoord);

		Vec3 diffuse = std::max(0.0f, Dot(worldNormal, lightDir)) * uniforms.LightDiffuse * diffColor;
		Vec3 specular = (float)std::pow(std::max(0.0f, Dot(worldNormal, halfDir)), uniforms.Shininess) * uniforms.LightSpecular * specularStrength;

		Vec3 result = (ambient + diffuse + specular);

		return Vec4(result, 1.0f);
	}

}

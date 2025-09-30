#include "PBRShader.h"

namespace RTL {

	void PBRVertexShader(PBRVaryings& varyings, const PBRVertex& vertex, const PBRUniforms& uniforms) {
		varyings.ClipPos = uniforms.MVP * vertex.ModelPos;
		varyings.TexCoord = vertex.TexCoord;
		varyings.WorldPos = uniforms.Model * vertex.ModelPos;
		varyings.WorldNormal = uniforms.ModelNormalWorld * Vec4(vertex.ModelNormal, 1.0f);
	}

	static float DistributionGGX(Vec3 N, Vec3 H, float roughness) {
		float a = roughness * roughness;
		float a2 = a * a;
		float NdotH = Max(0.0f, Dot(N, H));
		float NdotH2 = NdotH * NdotH;

		float nom = a2;
		float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
		denom = PI * denom * denom;

		return nom / denom;
	}

	static float GeometrySchlickGGX(float NdotV, float roughness) {
		float r = (roughness + 1.0f);
		float k = (r * r) / 8.0f;

		float nom = NdotV;
		float denom = NdotV * (1.0f - k) + k;

		return nom / denom;
	}

	static float GeometrySmith(Vec3 N, Vec3 V, Vec3 L, float roughness) {
		float NdotV = Max(0.0f, Dot(N, V));
		float NdotL = Max(0.0f, Dot(N, L));
		float ggx2 = GeometrySchlickGGX(NdotV, roughness);
		float ggx1 = GeometrySchlickGGX(NdotL, roughness);

		return ggx1 * ggx2;
	}

	static Vec3 FresnelSchlick(float cosTheta, Vec3 F0) {
		return F0 + (Vec3(1.0f, 1.0f, 1.0f) - F0) * pow(Clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
	}

	static Vec3 GammaCorrection(const Vec3& color) {
		float x = pow(color.X, 1.0f / 2.2f);
		float y = pow(color.Y, 1.0f / 2.2f);
		float z = pow(color.Z, 1.0f / 2.2f);
		return Vec3(x, y, z);
	}

	Vec4 PBRFragmentShader(bool& discard, const PBRVaryings& varyings, const PBRUniforms& uniforms) {
		Vec3 N = Normalize(varyings.WorldNormal);
		Vec3 V = Normalize(uniforms.CameraPos - varyings.WorldPos);

		Vec3 F0 = Vec3(0.04f, 0.04f, 0.04f);
		F0 = Lerp(F0, uniforms.Albedo, uniforms.Metalic);

		Vec3 Lo = Vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < uniforms.Lights.size(); i++) {
			Vec3 L = Normalize(uniforms.Lights[i].Position - varyings.WorldPos);
			Vec3 H = Normalize(L + V);
			float distance = Length(uniforms.Lights[i].Position - varyings.WorldPos);
			float attenuation = 1.0f / (distance * distance);
			Vec3 radiance = uniforms.Lights[i].Color * attenuation;

			float NDF = DistributionGGX(N, H, uniforms.Roughness);
			float G = GeometrySmith(N, V, L, uniforms.Roughness);
			Vec3 F = FresnelSchlick(Max(0.0f, Dot(H, V)), F0);

			Vec3 numerator = NDF * G * F;
			float denominator = 4.0f * Max(0.0f, Dot(N, V)) * Max(0.0f, Dot(N, L)) + 0.001f;
			Vec3 specular = numerator / denominator;

			Vec3 kS = F;
			Vec3 kD = Vec3(1.0f) - kS;
            kD *= 1.0f - uniforms.Metalic;

			float NdotL = Max(0.0f, Dot(N, L));

			Lo += (kD * uniforms.Albedo / PI + specular) * radiance * NdotL;
		}

		Vec4 Diffuse;
		if (uniforms.Diffuse)
			Diffuse = uniforms.Diffuse->Sample(varyings.TexCoord, uniforms.EnableLerpTexture);
		else
			Diffuse = Vec4(uniforms.Albedo, 1.0f);

		Vec3 ambient = Vec3(0.3f, 0.3f, 0.3f) * Diffuse;
		Vec3 color = ambient + Lo;
		color = color / (color + Vec3(1.0f));
		
		discard = false;
		return Vec4(color, 1.0f);
	}

	void PBROnUpdate(PBRUniforms& uniforms) {
		uniforms.ModelNormalWorld = Mat4Identity();
	}
	void PBRInit(PBRUniforms& uniforms) {
		uniforms.Diffuse = new Texture("../../assets/Test.png");
	}

}

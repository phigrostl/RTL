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
		const Vec3 Albedo = uniforms.Albedo->Sample(varyings.TexCoord, uniforms.EnableLerpTexture);

		const Vec3 MetallicVec = uniforms.Metallic->Sample(varyings.TexCoord, uniforms.EnableLerpTexture, Vec4(0.7f));
		const float Metallic = (MetallicVec.X + MetallicVec.Y + MetallicVec.Z) / 3.0f;

		const Vec3 RoughnessVec = uniforms.Roughness->Sample(varyings.TexCoord, uniforms.EnableLerpTexture, Vec4(0.5f));
		const float Roughness = (RoughnessVec.X + RoughnessVec.Y + RoughnessVec.Z) / 3.0f;

		const Vec3 AoVec = uniforms.Ao->Sample(varyings.TexCoord, uniforms.EnableLerpTexture, Vec4(1.0f));
		const float Ao = (AoVec.X + AoVec.Y + AoVec.Z) / 3.0f;

		Vec3 N = Normalize(varyings.WorldNormal);
		Vec3 V = Normalize(uniforms.CameraPos - varyings.WorldPos);

		Vec3 F0 = Vec3(0.04f, 0.04f, 0.04f);
		F0 = Lerp(F0, Albedo, Metallic);

		Vec3 Lo = Vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < uniforms.Lights.size(); i++) {
			Vec3 L = Normalize(uniforms.Lights[i].Position - varyings.WorldPos);
			Vec3 H = Normalize(L + V);
			float distance = Length(uniforms.Lights[i].Position - varyings.WorldPos);
			float attenuation = 1.0f / (distance * distance);
			Vec3 radiance = uniforms.Lights[i].Color;

			float NDF = DistributionGGX(N, H, Roughness);
			float G = GeometrySmith(N, V, L, Roughness);
			Vec3 F = FresnelSchlick(Max(0.0f, Dot(H, V)), F0);

			Vec3 numerator = NDF * G * F;
			float denominator = 4.0f * Max(0.0f, Dot(N, V)) * Max(0.0f, Dot(N, L)) + 0.001f;
			Vec3 specular = numerator / denominator;

			Vec3 kS = F;
			Vec3 kD = Vec3(1.0f) - kS;
            kD *= 1.0f - Metallic;

			float NdotL = Max(0.0f, Dot(N, L));

			Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
		}

		Vec3 ambient = Vec3(0.4f, 0.4f, 0.4f) * Albedo * Ao;
		Vec3 color = ambient + Lo;
		color = color / (color + Vec3(1.0f));
		
		discard = false;
		return Vec4(color, 1.0f);
	}

	void PBROnUpdate(PBRUniforms& uniforms) {
		uniforms.ModelNormalWorld = Mat4Identity();
		
	}
	void PBRInit(PBRUniforms& uniforms) {
		uniforms.EnableLerpTexture = false;

		uniforms.Lights.push_back(PBRLight());
		uniforms.Lights[0].Color = Vec3(1.0f, 1.0f, 1.0f);
		uniforms.Lights[0].Position = Vec3(0.0f, 0.0f, -1.5f);

		uniforms.Albedo = new Texture("../../assets/H.png");
		uniforms.Roughness = new Texture("../../assets/H.png");
		uniforms.Ao = new Texture("../../assets/H.png");
	}

}

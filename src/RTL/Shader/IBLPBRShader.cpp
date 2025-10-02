#include "IBLPBRShader.h"

namespace RTL {

	void IBLPBRVertexShader(IBLPBRVaryings& varyings, const IBLPBRVertex& vertex, const IBLPBRUniforms& uniforms) {
        varyings.ClipPos = uniforms.MVP * vertex.ModelPos;
        varyings.TexCoord = vertex.TexCoord;
        varyings.WorldPos = uniforms.Model * vertex.ModelPos;
        varyings.WorldNormal = uniforms.ModelNormalWorld * Vec4(vertex.ModelNormal, 1.0f);
        varyings.TexPos = vertex.ModelPos;
    }

    static float DistributionGGX(Vec3 N, Vec3 H, float roughness) {
        float a = roughness * roughness;
        float a2 = a * a;
        float NDotH = Max(Dot(N, H), 0.0f);
        float NDotH2 = NDotH * NDotH;

        float nom = a2;
        float denom = (NDotH2 * (a2 - 1.0f) + 1.0f);
        denom = PI * denom * denom;

        return nom / denom;
    }

    static float GeometrySchlickGGX(float NDotV, float roughness) {
        float r = (roughness + 1.0f);
        float k = (r * r) / 8.0f;

        float nom = NDotV;
        float denom = NDotV * (1.0f - k) + k;

        return nom / denom;
    }

    static float GeometrySmith(Vec3 N, Vec3 V, Vec3 L, float roughness) {
        float NDotV = Max(Dot(N, V), 0.0f);
        float NDotL = Max(Dot(N, L), 0.0f);
        float ggx2 = GeometrySchlickGGX(NDotV, roughness);
        float ggx1 = GeometrySchlickGGX(NDotL, roughness);

        return ggx1 * ggx2;
    }

    static Vec3 fresnelSchlick(float cosTheta, Vec3 F0) {
        return F0 + (1.0f - F0) * pow(Clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    }

    static Vec3 fresnelSchlickRoughness(float cosTheta, Vec3 F0, float roughness) {
        Vec3 v = Vec3(1.0f - roughness);
        v.X = Max(v.X, F0.X);
        v.Y = Max(v.Y, F0.Y);
        v.Z = Max(v.Z, F0.Z);
        Vec3 color = F0 + ((v - F0) * std::pow(Clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f));
        return color;
    }

    Vec4 IBLPBRFragmentShader(bool& discard, const IBLPBRVaryings& varyings, const IBLPBRUniforms& uniforms) {
        Vec3 N = Normalize(varyings.WorldNormal);
        Vec3 V = Normalize(uniforms.CamPos - varyings.WorldPos);
        Vec3 R = Reflect(-1 * V, N);

        float roughness = uniforms.Roughness;

        Vec3 F0 = Vec3(0.04f);
        F0 = Lerp(F0, uniforms.Albedo, uniforms.Metallic);

        Vec3 Lo = Vec3(0.0f);

#if true
        {
            Vec3 L = Normalize(uniforms.LightPos - varyings.WorldPos);
            Vec3 H = Normalize(V + L);
            float distance = Length(uniforms.LightPos - varyings.WorldPos);
            float attenuation = 1.0f / (distance * distance);
            Vec3 radiance = uniforms.LightColor * attenuation;

            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            Vec3 F = fresnelSchlick(Max(Dot(H, V), 0.0f), F0);

            Vec3 numerator = NDF * G * F;
            float denominator = 4.0f * Max(Dot(N, V), 0.0f) * Max(Dot(N, L), 0.0f) + 0.0001f;
            Vec3 specular = numerator / denominator;

            Vec3 kS = F;
            Vec3 kD = Vec3(1.0f) - kS;
            kD *= 1.0f - uniforms.Metallic;

            float NDotL = Max(Dot(N, L), 0.0f);

            Lo += (kD * uniforms.Albedo / PI + specular) * radiance * NDotL;
        }

#endif

        float NoV = Dot(N, V);
        Vec3 F = fresnelSchlickRoughness(Max(NoV, 0.0f), F0, roughness);

        Vec3 kS = F;
        Vec3 kD = 1.0f - kS;
        kD *= 1.0f - uniforms.Metallic;

        Vec3 irradiance = uniforms.IrradianceMap->Sample(varyings.TexPos);
        Vec3 diffuse = irradiance * uniforms.Albedo;

        constexpr float Max_REFLECTION_LOD = 4.0f;
        Vec3 prefilteredColor = uniforms.PrefilterMap->Sample(R, roughness * 4.0f);
        Vec2 brdf = uniforms.BrdfLUT->Sample(Vec2(Max(NoV, 0.0f), roughness));
        Vec3 specular = prefilteredColor * (F * brdf.X + brdf.Y);

        Vec3 ambient = (kD * diffuse + specular) * uniforms.Ao;

        Vec3 color = ambient + Lo;

        color = color / (color + Vec3(1.0f));
        constexpr float gamma = 1.0f / 2.2f;
        color = Pow(color, gamma);

        return Vec4{ color, 1.0f };
    }

    void IBLPBROnUpdate(IBLPBRUniforms& uniforms) {
        uniforms.ModelNormalWorld = Mat4Identity();
        uniforms.ModelMatrix = Mat4Identity();
        uniforms.NormalMatrix = Mat4Identity();
    }
    void IBLPBRInit(IBLPBRUniforms& uniforms) {
        uniforms.IrradianceMap = new TextureSphere("../../assets/Test.png");
        uniforms.PrefilterMap = new LodTextureSphere("../../assets/test.png");
        uniforms.BrdfLUT = new Texture("../../assets/box.png");
    }

}

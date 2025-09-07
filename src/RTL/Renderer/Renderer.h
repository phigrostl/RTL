#include "../Base/Maths.h"
#include "../Window/Framebuffer.h"

#define RTL_MAX_VARYINGS 9

namespace RTL {
	template<typename vertex_t>
	struct Triangle {
		vertex_t Vertex[3];

		vertex_t& operator[](int index){ return Vertex[index]; }
		const vertex_t& operator[](int index) const { return Vertex[index]; }

		Triangle() = default;
	};

	enum class DepthFuncType {
		LESS,
		LEQUAL,
		ALWAYS,
	};

	template<typename vertex_t, typename uniforms_t, typename varyings_t>
	struct Program {

		bool EnableDoubleSided = false;

		bool EnableDepthTest = true;
		bool EnableWriteDepth = true;

		bool EnableBlend = true;

		DepthFuncType DepthFunc = DepthFuncType::LESS;

		using vertex_shader_t = void (*)(varyings_t&, const vertex_t&, const uniforms_t&);
		vertex_shader_t VertexShader;

		using fragment_shader_t = Vec4 (*)(bool& discard, const varyings_t&, const uniforms_t&);
		fragment_shader_t FragmentShader;

		Program(const vertex_shader_t vertexShader, const fragment_shader_t fragmentShader)
			: VertexShader(vertexShader), FragmentShader(fragmentShader) {}
	};

	class Renderer {
	private:

		enum class Plane
		{
			POSITIVE_X,
			NEGATIVE_X,
			POSITIVE_Y,
			NEGATIVE_Y,
			POSITIVE_Z,
			NEGATIVE_Z,
		};

		struct BoundingBox {int MinX, MaxX, MinY, MaxY; };

		static bool IsInsidePlane(const Vec4& clipPos, const Plane plane);
		static bool IsVertexVisible(const Vec4& clipPos);
		static bool IsInsideTriangle(float(&weights)[3]);
		static bool PassDepthTest(const float weiteDepth, const float fDepth, const DepthFuncType depthFunc);

		static float GetIntersectRatio(const Vec4& prev, const Vec4& curr, const Plane plane);
		static BoundingBox GetBoundingBox(const Vec4(&fragCoords)[3], const int width, const int height);
		static void CalculateWeights(float(&screenWeights)[3], float(&weights)[3], const Vec4(&fragCoords)[3], const Vec2& screenPoint);

		template<typename varyings_t>
		static void LerpVaryings(varyings_t& out,
			const varyings_t& start,
			const varyings_t& end,
			const float ratio) {

			constexpr uint32_t floatNum = sizeof(varyings_t) / sizeof(float);
			float* startFloat = (float*)&start;
			float* endFloat = (float*)&end;
			float* outFloat = (float*)&out;

			for (int i = 0; i < (int)floatNum; i++)
				outFloat[i] = Lerp(startFloat[i], endFloat[i], ratio);

		}

		template<typename varyings_t>
		static void LerpVaryings(varyings_t& out,
			const varyings_t(&varyings)[3],
			const float(&weights)[3],
			const uint32_t width,
			const uint32_t height) {

			out.ClipPos = varyings[0].ClipPos * weights[0] +
				varyings[1].ClipPos * weights[1] +
				varyings[2].ClipPos * weights[2];

			out.NdcPos = out.ClipPos / out.ClipPos.W;
			out.NdcPos.W = 1.0f / out.ClipPos.W;

			out.FragPos.X = ((out.NdcPos.X + 1.0f) / 2.0f) * width;
			out.FragPos.Y = ((out.NdcPos.Y + 1.0f) / 2.0f) * height;
			out.FragPos.Z = (out.NdcPos.Z + 1.0f) / 2.0f;
			out.FragPos.W = out.NdcPos.W;

		}

		template<typename varyings_t>
		static int ClipAgainstPlane(varyings_t(&outVaryings)[RTL_MAX_VARYINGS],
			const varyings_t(&inVaryings)[RTL_MAX_VARYINGS],
			const Plane plane,
			const int inVertexNum) {

            int outVertexNum = 0;
			for (int i = 0; i < inVertexNum; i++) {
				int prevIndex = (i - 1 + inVertexNum) % inVertexNum;
				int currIndex = i;

				const varyings_t& prevVaryings = inVaryings[prevIndex];
				const varyings_t& currVaryings = inVaryings[currIndex];

				const bool prevInside = IsInsidePlane(prevVaryings.ClipPos, plane);
				const bool currInside = IsInsidePlane(currVaryings.ClipPos, plane);

				if (currInside != prevInside)
				{
					float ratio = GetIntersectRatio(prevVaryings.ClipPos, currVaryings.ClipPos, plane);
					LerpVaryings(outVaryings[outVertexNum], prevVaryings, currVaryings, ratio);
					outVertexNum++;
				}

				if (currInside)
				{
					outVaryings[outVertexNum] = inVaryings[currIndex];
					outVertexNum++;
				}
			}

			return outVertexNum;
		}

		template<typename varyings_t>
		static int Clip(varyings_t(&varyings)[RTL_MAX_VARYINGS])
		{
			bool v0_Visible = IsVertexVisible(varyings[0].ClipPos);
			bool v1_Visible = IsVertexVisible(varyings[1].ClipPos);
			bool v2_Visible = IsVertexVisible(varyings[2].ClipPos);
			if (v0_Visible && v1_Visible && v2_Visible)
				return 3;

			int vertexNum = 3;
			varyings_t varyings_[RTL_MAX_VARYINGS];
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::NEGATIVE_Z, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::POSITIVE_Z, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::POSITIVE_X, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::NEGATIVE_X, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::POSITIVE_Y, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::NEGATIVE_Y, vertexNum);
			if (vertexNum == 0) return 0;

			return vertexNum;
		}

		template<typename varyings_t>
		static void CaculateNdcPos(varyings_t(&varyings)[RTL_MAX_VARYINGS], const int vertexNum)
		{
			for (int i = 0; i < vertexNum; i++)
			{
				float w = varyings[i].ClipPos.W;
				varyings[i].NdcPos = varyings[i].ClipPos / w;
				varyings[i].NdcPos.W = 1.0f / w;
			}
		}

		template<typename varyings_t>
		static void CaculateFragPos(varyings_t(&varyings)[RTL_MAX_VARYINGS],
			const int vertexNum,
			const float width,
			const float height) {

			for (int i = 0; i < vertexNum; i++)
			{
				float x = ((varyings[i].NdcPos.X + 1.0f) * 0.5f * width);
				float y = ((varyings[i].NdcPos.Y + 1.0f) * 0.5f * height);
				float z = (varyings[i].NdcPos.Z + 1.0f) * 0.5f;
				float w = varyings[i].NdcPos.W;

				varyings[i].FragPos.X = x;
				varyings[i].FragPos.Y = y;
				varyings[i].FragPos.Z = z;
				varyings[i].FragPos.W = w;
			}
		}

		template<typename varyings_t>
		static bool IsBackFacing(const varyings_t(&fragCoords)[3]) {
			float signedArea = fragCoords[0].NdcPos.X * fragCoords[1].NdcPos.Y - fragCoords[0].NdcPos.Y * fragCoords[1].NdcPos.X +
				fragCoords[1].NdcPos.X * fragCoords[2].NdcPos.Y - fragCoords[1].NdcPos.Y * fragCoords[2].NdcPos.X +
				fragCoords[2].NdcPos.X * fragCoords[0].NdcPos.Y - fragCoords[2].NdcPos.Y * fragCoords[0].NdcPos.X;
			return signedArea <= 0.0f;
		}

		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void ProcessPixel(Framebuffer& framebuffer,
			const int x, const int y,
			const Program<vertex_t, uniforms_t, varyings_t>& program,
			const varyings_t& varyings,
			const uniforms_t& uniforms) {

			bool discard = false;
			Vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
			color = program.FragmentShader(discard, varyings, uniforms);
			if (discard) return;

			color.X = Clamp(color.X, 0.0f, 1.0f);
			color.Y = Clamp(color.Y, 0.0f, 1.0f);
			color.Z = Clamp(color.Z, 0.0f, 1.0f);
			color.W = Clamp(color.W, 0.0f, 1.0f);

			if (program.EnableBlend) {
				Vec3 dstColor = framebuffer.GetColor(x, y);
				Vec3 srcColor = color;
				float alpha = color.W;
				color = { Lerp(dstColor, srcColor, alpha), 1.0f };
			}

			framebuffer.SetColor(x, y, color);

			if (program.EnableWriteDepth) {
				float depth = varyings.FragPos.Z;
				framebuffer.SetDepth(x, y, depth);
			}
		}

		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void RasterizeTriangle(Framebuffer& framebuffer,
			const Program<vertex_t, uniforms_t, varyings_t>& program,
			const varyings_t(&varyings)[3],
			const uniforms_t& uniforms) {


			if (!program.EnableDoubleSided) {
				bool isBackFacing = IsBackFacing(varyings);
				if (isBackFacing) return;
			}

			int fWidth = framebuffer.GetWidth();
			int fHeight = framebuffer.GetHeight();

			Vec4 fragCoords[3];
			fragCoords[0] = varyings[0].FragPos;
			fragCoords[1] = varyings[1].FragPos;
			fragCoords[2] = varyings[2].FragPos;
			BoundingBox bBox = GetBoundingBox(fragCoords, fWidth, fHeight);

			for (int y = bBox.MinY; y <= bBox.MaxY; y++) {
				for (int x = bBox.MinX; x <= bBox.MaxX; x++) {
					float screenWeights[3];
					float weights[3];
					Vec2 screenPoint{ (float)x + 0.5f, (float)y + 0.5f };

					CalculateWeights(screenWeights, weights, fragCoords, screenPoint);
					if(!IsInsideTriangle(weights))
						continue;

					varyings_t pixVaryings;
					LerpVaryings(pixVaryings, varyings, weights, fWidth, fHeight);

					if (program.EnableDepthTest) {
						float depth = pixVaryings.FragPos.Z;
						float fDepth = framebuffer.GetDepth(x, y);
						DepthFuncType depthFunc = program.DepthFunc;
						if (!PassDepthTest(depth, fDepth, depthFunc))
							continue;
					}

					ProcessPixel(framebuffer, x, y, program, pixVaryings, uniforms);
					
				}
			}

		}

    public:

		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void Draw(Framebuffer& framebuffer, const Program<vertex_t, uniforms_t, varyings_t>& program, const Triangle<vertex_t>& triangle, const uniforms_t& uniforms) {

			varyings_t varyings[RTL_MAX_VARYINGS];
			for (int i = 0; i < 3; i++)
				program.VertexShader(varyings[i], triangle[i], uniforms);

			int vertexNum = Clip(varyings);

			CaculateNdcPos(varyings, vertexNum);
			int fWidth = framebuffer.GetWidth();
			int fHeight = framebuffer.GetHeight();
			CaculateFragPos(varyings, vertexNum, (float)fWidth, (float)fHeight);

			for (int i = 0; i < vertexNum - 2; i++) {
				varyings_t triVaryings[3];
				triVaryings[0] = varyings[0];
				triVaryings[1] = varyings[i + 1];
				triVaryings[2] = varyings[i + 2];

				RasterizeTriangle(framebuffer, program, triVaryings, uniforms);
			}

		}

	};

}

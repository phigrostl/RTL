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

	template<typename vertex_t, typename uniforms_t, typename varyings_t>
	struct Program {
		using vertex_shader_t = void (*)(varyings_t&, const vertex_t&, const uniforms_t&);
		vertex_shader_t VertexShader;

		Program(const vertex_shader_t vertexShader) : VertexShader(vertexShader) {}
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

		static bool IsInsidePlane(const Vec4& clipPos, const Plane plane);
		static bool IsVertexVisible(const Vec4& clipPos);

		static float GetIntersectRatio(const Vec4& prev, const Vec4& curr, const Plane plane);

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

			constexpr uint32_t floatOffset = sizeof(Vec4) * 3 / sizeof(float);
			constexpr uint32_t floatNum = sizeof(varyings_t) / sizeof(float);
			float* v0 = (float*)&varyings[0];
			float* v1 = (float*)&varyings[1];
			float* v2 = (float*)&varyings[2];
			float* outFloat = (float*)&out;

			for (int i = floatOffset; i < (int)floatNum; i++)
				outFloat[i] = v0[i] * weights[0] +
				v1[i] * weights[1] +
				v2[i] * weights[2];

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

		
    public:

		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void Draw(Framebuffer& framebuffer, const Program<vertex_t, uniforms_t, varyings_t>& program, const Triangle<vertex_t>& triangle, const uniforms_t& uniforms) {

			varyings_t varyings[RTL_MAX_VARYINGS];
			for (int i = 0; i < 3; i++)
				program.VertexShader(varyings[i], triangle[i], uniforms);

			int vertexNum = Clip(varyings);

		}

	};

}

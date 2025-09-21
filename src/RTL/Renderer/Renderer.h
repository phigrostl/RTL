#include "RTL/Base/Maths.h"
#include "RTL/Window/Framebuffer.h"

#include <memory>

#define RTL_MAX_VARYINGS 9

namespace RTL {

	template<typename vertex_t>
	struct Triangle {
		vertex_t Vertex[3];

		vertex_t operator[](size_t index) { return Vertex[index]; }
		const vertex_t& operator[](size_t index) const { return Vertex[index]; }

		Triangle() = default;
	};

	template<typename vertex_t, typename uniforms_t, typename varyings_t>
	struct Program {
		using vertex_shader_t = void (*)(varyings_t&, const vertex_t&, const uniforms_t&);

		vertex_shader_t VertexShader;

		Program(const vertex_shader_t vertexShader)
			: VertexShader(vertexShader) {}
	};

	class Renderer {
	private:

		enum class Plane {
            POSITIVE_W,
			POSITIVE_X,
			NEGATIVE_X,
			POSITIVE_Y,
			NEGATIVE_Y,
			POSITIVE_Z,
			NEGATIVE_Z
		};

		template <typename varyings_t>
		static void LerpVaryings(varyings_t& out, const varyings_t& start, const varyings_t& end, float ratio) {
			constexpr uint32_t floatNum = sizeof(varyings_t) / sizeof(float);
			float* startFloat = (float*)&start;
			FLOAT* endFloat = (float*)&end;
			FLOAT* outFloat = (float*)&out;
			for (uint32_t i = 0; i < floatNum; i++)
				outFloat[i] = Lerp(startFloat[i], endFloat[i], ratio);
		}

		static bool IsVertexVisible(const Vec4& clipPos) {
			return std::fabs(clipPos.X) <= clipPos.W && std::fabs(clipPos.Y) <= clipPos.W && std::fabs(clipPos.Z) <= clipPos.W;
		}

		static bool IsInsidePlane(const Vec4& clipPos, const Plane plane) {
			switch (plane) {
			case Plane::POSITIVE_W:
				return clipPos.W >= 0.0f;
			case Plane::POSITIVE_X:
				return clipPos.X <= clipPos.W;
			case Plane::NEGATIVE_X:
				return clipPos.X >= -clipPos.W;
			case Plane::POSITIVE_Y:
				return clipPos.Y <= clipPos.W;
			case Plane::NEGATIVE_Y:
				return clipPos.Y >= -clipPos.W;
			case Plane::POSITIVE_Z:
				return clipPos.Z <= clipPos.W;
			case Plane::NEGATIVE_Z:
				return clipPos.Z >= -clipPos.W;
			default:
				return false;
			}
		}

		static float GetIntersectRatio(const Vec4& prev, const Vec4& curr, const Plane plane) {
			switch (plane) {
			case Plane::POSITIVE_W:
				return (prev.W - 0.0f) / (prev.W - curr.W);
			case Plane::POSITIVE_X:
				return (prev.W - prev.X) / ((prev.W - prev.X) - (curr.W - curr.X));
			case Plane::NEGATIVE_X:
				return (prev.W + prev.X) / ((prev.W + prev.X) - (curr.W + curr.X));
			case Plane::POSITIVE_Y:
				return (prev.W - prev.Y) / ((prev.W - prev.Y) - (curr.W - curr.Y));
			case Plane::NEGATIVE_Y:
				return (prev.W + prev.Y) / ((prev.W + prev.Y) - (curr.W + curr.Y));
			case Plane::POSITIVE_Z:
				return (prev.W - prev.Z) / ((prev.W - prev.Z) - (curr.W - curr.Z));
			case Plane::NEGATIVE_Z:
				return (prev.W + prev.Z) / ((prev.W + prev.Z) - (curr.W + curr.Z));
			default:
				return 0.0f;
			}
		}

		template<typename varyings_t>
		static int ClipAgainstPlane(varyings_t(&outVaryings)[RTL_MAX_VARYINGS],
									const varyings_t(&inVaryings)[RTL_MAX_VARYINGS],
									const Plane plane,
									const int inVertexNum) {

			int outVertexNum = 0;
			for (int i = 0; i < inVertexNum; i++) {
				int prevIndex = (inVertexNum - 1 + i) % inVertexNum;
				int currIndex = i;

				const varyings_t& prevVarying = inVaryings[prevIndex];
				const varyings_t& currVarying = inVaryings[currIndex];

				const bool prevInside = IsInsidePlane(prevVarying.ClipPos, plane);
				const bool currInside = IsInsidePlane(currVarying.ClipPos, plane);

				if (prevInside != currInside) {
					float ratio = GetIntersectRatio(prevVarying.ClipPos, currVarying.ClipPos, plane);
					LerpVaryings(outVaryings[outVertexNum], prevVarying, currVarying, ratio);
					outVertexNum++;
				}

				if (currInside) {
					outVaryings[outVertexNum] = inVaryings[currIndex];
					outVertexNum++;
				}
			}
			return outVertexNum;
		}

		template<typename varyings_t>
		static int Clip(varyings_t(&varyings)[RTL_MAX_VARYINGS]) {
			bool v0_Visible = IsVertexVisible(varyings[0].ClipPos);
			bool v1_Visible = IsVertexVisible(varyings[1].ClipPos);
			bool v2_Visible = IsVertexVisible(varyings[2].ClipPos);
			if (v0_Visible && v1_Visible && v2_Visible)
				return 3;

			int vertexNum = 3;
			varyings_t varyings_[RTL_MAX_VARYINGS];
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::POSITIVE_W, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::POSITIVE_X, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::NEGATIVE_X, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::POSITIVE_Y, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::NEGATIVE_Y, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::POSITIVE_Z, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::NEGATIVE_Z, vertexNum);
			if (vertexNum == 0) return 0;
			memcpy(varyings, varyings_, sizeof(varyings_));

			return vertexNum;
		}

	public:
		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void Draw(Framebuffer* framebuffer, const Program<vertex_t, uniforms_t, varyings_t>& program, const Triangle<vertex_t>& triangle, const uniforms_t& uniforms) {
			varyings_t varyings[RTL_MAX_VARYINGS];
			for (int i = 0; i < 3; i++)
				program.VertexShader(varyings[i], triangle[i], uniforms);

			int vertexNum = Clip(varyings);
		}
	};

}

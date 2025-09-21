#include "BlinnShader.h"

namespace RTL {

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms) {
		varyings.ClipPos = uniforms.MVP * vertex.ModelPos;
	}

	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms) {
		discard = false;
		return { varyings.NdcPos.X / 2 + 0.5f, varyings.NdcPos.Y / 2 + 0.5f, 0.0f, 1.0f };
	}

}
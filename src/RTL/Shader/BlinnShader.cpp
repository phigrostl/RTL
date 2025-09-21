#include "BlinnShader.h"

namespace RTL {

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms) {
		varyings.ClipPos = uniforms.MVP * vertex.ModelPos;
	}

	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms) {
		discard = false;
		return { varyings.NdcPos.Z / 2 + 0.5f, 1.0f, 1.0f, 1.0f };
	}

}
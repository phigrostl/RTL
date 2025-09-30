#include "RTL/Application.h"

#include "RTL/Shader/BlinnShader.h"

using namespace RTL;

int main(int argc, char* argv[]) {

	Application<BlinnVertex, BlinnVaryings, BlinnUniforms> App(
		argc, argv,
		"RTL", 800, 600,
		BlinnVertexShader, BlinnFragmentShader,
		BlinnInit, BlinnOnUpdate
	);

	App.Run();

	return 0;
}

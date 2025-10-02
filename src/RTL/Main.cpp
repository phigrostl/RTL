#include "RTL/Application.h"

#include "RTL/Shader/BlinnShader.h"

using namespace RTL;

int main(int argc, char* argv[]) {

	Application App(
		argc, argv,
		"RTL", 600, 400,
		BlinnVertexShader, BlinnFragmentShader,
		BlinnInit, BlinnOnUpdate
	);

	App.Run();

	return 0;
}

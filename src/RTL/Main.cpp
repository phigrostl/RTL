#include "RTL/Application.h"

#include "RTL/Shader/PBRShader.h"

using namespace RTL;

int main(int argc, char* argv[]) {

	Application App(
		argc, argv,
		"RTL", 1280, 960,
		PBRVertexShader, PBRFragmentShader,
		PBRInit, PBROnUpdate
	);

	App.Run();

	return 0;
}

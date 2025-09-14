#include "Application.h"

using namespace RTL;

int main(int argc, char* argv[]) {

	Application App(argc, argv, "RTL", 800, 600);

	App.Run();

	return 0;
}

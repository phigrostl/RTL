cmake_minimum_required (VERSION 3.8)

project ("RTL")

set(CMAKE_CXX_STANDARD 17)

add_executable (RTL
	"src/RTL/Main.cpp"

	"src/RTL/Application.h"
	"src/RTL/Application.cpp"

	"src/Window/Window.cpp"
	"src/Window/Window.h"

	"src/Window/Framebuffer.h"
	"src/Window/Framebuffer.cpp"

	"src/Base/Base.h"

	"src/Base/Maths.h"
	"src/Base/Maths.cpp"

	"src/Window/InputCodes.h"
)

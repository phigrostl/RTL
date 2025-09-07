cmake_minimum_required (VERSION 3.8)

project ("RTL")

set(CMAKE_CXX_STANDARD 17)

add_executable (RTL
	"src/RTL/Main.cpp"

	"src/RTL/Application.h"
	"src/RTL/Application.cpp"

	"src/RTL/Window/Window.cpp"
	"src/RTL/Window/Window.h"

	"src/RTL/Window/Framebuffer.h"
	"src/RTL/Window/Framebuffer.cpp"

	"src/RTL/Base/Base.h"

	"src/RTL/Base/Maths.h"
	"src/RTL/Base/Maths.cpp"

	"src/RTL/Window/InputCodes.h"

	"src/RTL/Shaders/ShaderBase.h"
	
	"src/RTL/Renderer/Renderer.h"
	"src/RTL/Renderer/Renderer.cpp"

	"src/RTL/Shaders/BlinnShader.h"
	"src/RTL/Shaders/BlinnShader.cpp"
)

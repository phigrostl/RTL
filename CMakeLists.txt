cmake_minimum_required(VERSION 3.10)
project(RTL)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(RTL 
	"src/RTL/Main.cpp"
	"src/RTL/Application.h"
	"src/RTL/Window/Window.cpp"
	"src/RTL/Window/Framebuffer.cpp"
	"src/RTL/Base/Maths.cpp"
	"src/RTL/Shader/Texture.cpp"
	"src/RTL/Renderer/Renderer.cpp"

	"src/RTL/stb/stb_image.cpp"
	"src/RTL/stb/stb_truetype.cpp"
	
	"src/RTL/Shader/BlinnShader.cpp"
	"src/RTL/Shader/PBRShader.cpp"
)

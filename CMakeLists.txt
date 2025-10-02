cmake_minimum_required(VERSION 3.10)
project(RTL)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)

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
	"src/RTL/stb/std_image_resize2.cpp"
	
	"src/RTL/Shader/BlinnShader.cpp"
	"src/RTL/Shader/PBRShader.cpp"
	"src/RTL/Shader/BRDFShader.cpp"
	"src/RTL/Shader/IBLPBRShader.cpp"
)

target_link_libraries(RTL PRIVATE)

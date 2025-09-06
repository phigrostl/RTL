cmake_minimum_required (VERSION 3.8)

project ("RTL")

set(CMAKE_CXX_STANDARD 17)

add_executable (RTL
	"src/RTL/Main.cpp"
	"src/RTL/Application.cpp"
)

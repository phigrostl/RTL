cmake_minimum_required(VERSION 3.10)
project(RTL)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(RTL 
	"src/RTL/Main.cpp"
	"src/RTL/Application.cpp"
)

workspace "LearnOpenGL"
	architecture "x64"
	configurations { "Debug", "Release" }

binarydir = "bin"
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"	--e.g., Debug-Windows-x64

include "vendor/glad/premake5.lua"

glfwdir = "vendor/glfw"
os.execute(
	"cmake -S " .. glfwdir .. " -B " .. glfwdir .. "/build" 
	.. " -D " .. "GLFW_BUILD_EXAMPLES=OFF" 
	.. " -D " .. "GLFW_BUILD_TESTS=OFF" 
	.. " -D " .. "GLFW_BUILD_DOCS=OFF" 
	.. " -D " .. "USE_MSVC_RUNTIME_LIBRARY_DLL=OFF"
)
os.execute("cmake --build " .. glfwdir .. "/build")

project "FirstScene"
	location "FirstScene"
	kind "ConsoleApp"
	language "C++"
	cdialect "Default"
	cppdialect "C++20"
	systemversion "latest"
	staticruntime "On"

	objdir (binarydir .. "/tmp/" .. outputdir .. "/%{prj.name}")
	targetdir (binarydir .. "/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/Shader/**.h",
		"%{prj.name}/Shader/**.cpp",
		"%{prj.name}/Texture/**.h",
		"%{prj.name}/Texture/**.cpp",
		"%{prj.name}/Viewer/**.h",
		"%{prj.name}/Viewer/**.cpp",
		"%{prj.name}/macro.h",
		"%{prj.name}/main.cpp",
	}

	includedirs {
		"%{prj.name}",
		"vendor/glfw/include",
		"vendor/glad/include",
		"vendor/glm",
		"vendor/stb",
	}

	libdirs {
		"vendor/glfw/build/src/Debug",
	}

	links {
		"glad",
		"glfw3.lib",
		"opengl32.lib",
	}

	buildoptions {
		"/utf-8",
	}

	postbuildcommands {
		("{MKDIR} ../" .. binarydir .. "/" .. outputdir .. "/%{prj.name}/assets"),
		("{MKDIR} ../" .. binarydir .. "/" .. outputdir .. "/%{prj.name}/assets/shaders"),
		("{MKDIR} ../" .. binarydir .. "/" .. outputdir .. "/%{prj.name}/assets/textures"),
		("{COPYFILE} assets/shaders ../" .. binarydir .. "/" .. outputdir .. "/%{prj.name}/assets/shaders"),
		("{COPYFILE} assets/textures ../" .. binarydir .. "/" .. outputdir .. "/%{prj.name}/assets/textures"),
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
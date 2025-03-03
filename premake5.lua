workspace "LearnOpenGL"
	architecture "x64"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"	--e.g., Debug-Windows-x64

include "LearnOpenGL/third-party/glad/premake5.lua"

glfwdir = "LearnOpenGL/third-party/glfw"
os.execute(
	"cmake -S " .. glfwdir .. " -B " .. glfwdir .. "/build" 
	.. " -D " .. "GLFW_BUILD_EXAMPLES=OFF" 
	.. " -D " .. "GLFW_BUILD_TESTS=OFF" 
	.. " -D " .. "GLFW_BUILD_DOCS=OFF" 
	.. " -D " .. "USE_MSVC_RUNTIME_LIBRARY_DLL=OFF"
)
os.execute("cmake --build " .. glfwdir .. "/build")

project "LearnOpenGL"
	location "LearnOpenGL"
	kind "ConsoleApp"
	language "C++"
	cdialect "Default"
	cppdialect "C++20"
	systemversion "latest"
	staticruntime "On"

	objdir ("bin/tmp/" .. outputdir .. "/%{prj.name}")
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/third-party/glfw/include",
		"%{prj.name}/third-party/glad/include",
		"%{prj.name}/third-party/glm",
		"%{prj.name}/third-party/stb",
	}

	libdirs {
		"%{prj.name}/third-party/glfw/build/src/Debug",
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
		("{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/src"),
		("{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/src/Shaders"),
		("{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/src/Textures"),
		("{COPYFILE} src/Shaders ../bin/" .. outputdir .. "/%{prj.name}/src/Shaders/"),
		("{COPYFILE} src/Textures ../bin/" .. outputdir .. "/%{prj.name}/src/Textures/"),
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
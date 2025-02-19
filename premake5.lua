workspace "LearnOpenGL"
	architecture "x64"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"	--e.g., Debug-Windows-x64

include "LearnOpenGL/third-party/glfw/premake5.lua"
include "LearnOpenGL/third-party/glad/premake5.lua"

project "LearnOpenGL"
	location "LearnOpenGL"
	kind "ConsoleApp"
	language "C++"

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
	}

	links {
		"glfw",
		"glad",
		"opengl32.lib",
	}

	buildoptions {
		"/utf-8",
	}

	postbuildcommands {
		("{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/src"),
		("{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/src/shaders"),
		("{COPYFILE} src/shaders ../bin/" .. outputdir .. "/%{prj.name}/src/shaders/"),
	}

	filter "system:linux"
		pic "On"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
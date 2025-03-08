workspace "LearnOpenGL"
	architecture "x64"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"	--e.g., Debug-Windows-x64

-- GLAD --
include "vendor/glad/premake5.lua"

-- GLFW --
os.execute(
	"cmake -S vendor/glfw -B vendor/glfw/build"
	.. " -D " .. "GLFW_BUILD_EXAMPLES=OFF" 
	.. " -D " .. "GLFW_BUILD_TESTS=OFF" 
	.. " -D " .. "GLFW_BUILD_DOCS=OFF" 
	.. " -D " .. "USE_MSVC_RUNTIME_LIBRARY_DLL=OFF"
)

-- Assimp --
os.execute(
	"cmake -S vendor/assimp -B vendor/assimp/build"
	.. " -D " .. "ASSIMP_BUILD_ASSIMP_TOOLS=OFF"
	.. " -D " .. "BUILD_SHARED_LIBS=ON"
)

-- Build projects --
project "FirstScene"
	location "FirstScene"
	kind "ConsoleApp"
	language "C++"
	cdialect "Default"
	cppdialect "C++20"
	systemversion "latest"
	staticruntime "On"

	objdir ("bin/tmp/" .. outputdir .. "/%{prj.name}")
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/Shader/**.h",
		"%{prj.name}/Shader/**.cpp",
		"%{prj.name}/Texture/**.h",
		"%{prj.name}/Texture/**.cpp",
		"%{prj.name}/Viewer/**.h",
		"%{prj.name}/Viewer/**.cpp",
		"%{prj.name}/Model/**.h",
		"%{prj.name}/Model/**.cpp",
		"%{prj.name}/Framebuffer/**.h",
		"%{prj.name}/Framebuffer/**.cpp",
		"%{prj.name}/macro.h",
		"%{prj.name}/main.cpp",
	}

	includedirs {
		"%{prj.name}",
		"vendor/glad/include",
		"vendor/glfw/include",
		"vendor/glm",
		"vendor/stb",
		"vendor/assimp/include",
		"vendor/assimp/build/include",
	}

	libdirs {
		"vendor/glfw/build/src/%{cfg.buildcfg}",
		"vendor/assimp/build/bin/%{cfg.buildcfg}",
		"vendor/assimp/build/lib/%{cfg.buildcfg}",
	}

	buildoptions {
		"/utf-8",
	}

	buildcommands {
		"cmake --build vendor/glfw/build --config %{cfg.buildcfg}",
		"cmake --build vendor/assimp/build --config %{cfg.buildcfg}",
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		links {
			"glad",
			"glfw3",
			"assimp-vc143-mtd",
			"opengl32",
		}
		
		postbuildcommands {
			("{COPYFILE} ../vendor/assimp/build/bin/%{cfg.buildcfg}/assimp-vc143-mtd.dll ../bin/" .. outputdir .. "/%{prj.name}"),
			("{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/assets"),
			("{COPYDIR} assets ../bin/" .. outputdir .. "/%{prj.name}/assets"),
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

		links {
			"glad",
			"glfw3",
			"assimp-vc143-mt",
			"opengl32",
		}

		postbuildcommands {
			("{COPYFILE} ../vendor/assimp/build/bin/%{cfg.buildcfg}/assimp-vc143-mt.dll ../bin/" .. outputdir .. "/%{prj.name}"),
			("{MKDIR} ../bin/" .. outputdir .. "/%{prj.name}/assets"),
			("{COPYDIR} assets ../bin/" .. outputdir .. "/%{prj.name}/assets"),
		}
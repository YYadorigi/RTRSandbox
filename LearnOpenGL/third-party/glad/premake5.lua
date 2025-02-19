project "glad"
	kind "StaticLib"
	language "C"

	objdir ("bin/tmp/" .. outputdir .. "/%{prj.name}")
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")

	files {
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c",
	}

	includedirs {
		"include",
	}

	buildoptions {
		"/utf-8",
	}

	filter "system:linux"
		pic "On"
		cdialect "Default"
		staticruntime "On"
		systemversion "latest"

	filter "system:windows"
		cdialect "Default"
		staticruntime "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"